import streamlit as st
import pandas as pd
import plotly.express as px
import os
import glob
import re

st.set_page_config(page_title="Аналіз супутникових даних", layout="wide")

@st.cache_data
def load_data(data_dir="data"):
    if not os.path.exists(data_dir):
        st.error(f"Теку '{data_dir}' не знайдено! Запустіть скрипт завантаження даних.")
        return pd.DataFrame()
        
    all_files = glob.glob(os.path.join(data_dir, "vhi_id_*.csv"))
    if not all_files:
        st.error("У теці 'data' немає CSV файлів. Перевірте завантаження.")
        return pd.DataFrame()

    province_dict = {
        1: 'Вінницька', 2: 'Волинська', 3: 'Дніпропетровська', 4: 'Донецька',
        5: 'Житомирська', 6: 'Закарпатська', 7: 'Запорізька', 8: 'Івано-Франківська',
        9: 'Київська', 10: 'Кіровоградська', 11: 'Луганська', 12: 'Львівська',
        13: 'Миколаївська', 14: 'Одеська', 15: 'Полтавська', 16: 'Рівненська',
        17: 'Сумська', 18: 'Тернопільська', 19: 'Харківська', 20: 'Херсонська',
        21: 'Хмельницька', 22: 'Черкаська', 23: 'Чернівецька', 24: 'Чернігівська',
        25: 'Республіка Крим', 26: 'м. Київ', 27: 'м. Севастополь'
    }
    
    data_list = []
    
    for file in all_files:
        filename = os.path.basename(file)
        try:
            province_id = int(filename.split('_')[2])
        except (IndexError, ValueError):
            continue
            
        with open(file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.replace('<tt><pre>', '').replace('</pre></tt>', '').strip()
                if re.match(r'^(19|20)\d{2}', line):
                    parts = [p.strip() for p in line.split(',')]
                    if len(parts) >= 7:
                        year, week, smn, smt, vci, tci, vhi = parts[:7]
                        if float(vhi) != -1.0:
                            data_list.append([
                                int(year), int(week), float(vci), float(tci), float(vhi),
                                province_dict.get(province_id, f"Область {province_id}")
                            ])
                            
    df = pd.DataFrame(data_list, columns=['Year', 'Week', 'VCI', 'TCI', 'VHI', 'Region'])
    return df

df = load_data()

if df.empty:
    st.stop()

def reset_filters():
    st.session_state.index_sel = "VHI"
    st.session_state.region_sel = df['Region'].unique()[0]
    st.session_state.week_sel = (1, 52)
    st.session_state.year_sel = (int(df['Year'].min()), int(df['Year'].max()))
    st.session_state.sort_asc = False
    st.session_state.sort_desc = False

if 'index_sel' not in st.session_state:
    reset_filters()

col_filters, col_content = st.columns([1, 3])

with col_filters:
    st.header("Налаштування")
    
    index = st.selectbox("Оберіть часовий ряд:", ["VCI", "TCI", "VHI"], key='index_sel')
    region = st.selectbox("Оберіть область:", sorted(df['Region'].unique()), key='region_sel')
    
    min_year, max_year = int(df['Year'].min()), int(df['Year'].max())
    weeks = st.slider("Інтервал тижнів:", 1, 52, key='week_sel')
    years = st.slider("Інтервал років:", min_year, max_year, key='year_sel')
    
    st.markdown("---")
    st.subheader("Сортування")
    
    sort_asc = st.checkbox("За зростанням", key='sort_asc')
    sort_desc = st.checkbox("За спаданням", key='sort_desc')
    
    if sort_asc and sort_desc:
        st.warning("Увімкнено обидва чекбокси! Сортування скасовано.")
        sort_by = None
    elif sort_asc:
        sort_by = 'asc'
    elif sort_desc:
        sort_by = 'desc'
    else:
        sort_by = None

    st.markdown("---")
    st.button("Скинути фільтри", on_click=reset_filters, use_container_width=True)

filtered_df = df[
    (df['Year'] >= years[0]) & (df['Year'] <= years[1]) & 
    (df['Week'] >= weeks[0]) & (df['Week'] <= weeks[1])
]

region_df = filtered_df[filtered_df['Region'] == region].copy()


if sort_by == 'asc':
    region_df = region_df.sort_values(by=index, ascending=True)
elif sort_by == 'desc':
    region_df = region_df.sort_values(by=index, ascending=False)

with col_content:
    st.header(f"Аналіз індексу {index} для регіону: {region}")
    
    tab1, tab2, tab3 = st.tabs(["📋 Таблиця даних", "📈 Графік динаміки", "📊 Порівняння по областях"])
    
    with tab1:
        st.subheader("Відфільтровані дані")
        st.dataframe(region_df[['Year', 'Week', 'Region', index]], use_container_width=True)
        st.caption(f"Кількість записів: {len(region_df)}")
        
    with tab2:
        st.subheader(f"Динаміка {index} ({years[0]}-{years[1]})")
        
        plot_df = region_df.sort_values(by=['Year', 'Week']).copy()
        
        plot_df['Year-Week'] = plot_df['Year'].astype(str) + "-W" + plot_df['Week'].astype(str).str.zfill(2)
        
        fig1 = px.line(plot_df, x='Year-Week', y=index, 
                       title=f'Графік значень {index} у {region} області',
                       markers=True)
        st.plotly_chart(fig1, use_container_width=True)
        
    with tab3:
        st.subheader(f"Порівняння {index}: {region} vs Інші області")
        
        plot_df_chrono = plot_df.copy()
        plot_df_chrono['Type'] = region
        
        other_regions_df = filtered_df[filtered_df['Region'] != region]
        other_agg = other_regions_df.groupby(['Year', 'Week'])[index].mean().reset_index()
        other_agg['Year-Week'] = other_agg['Year'].astype(str) + "-W" + other_agg['Week'].astype(str).str.zfill(2)
        other_agg['Type'] = 'Середнє (всі інші області)'
        
        comp_df = pd.concat([
            plot_df_chrono[['Year-Week', index, 'Type']], 
            other_agg[['Year-Week', index, 'Type']]
        ])
        
        fig2 = px.line(comp_df, x='Year-Week', y=index, color='Type', 
                       title=f'Порівняння {index} із середнім по країні',
                       markers=True)
        st.plotly_chart(fig2, use_container_width=True)

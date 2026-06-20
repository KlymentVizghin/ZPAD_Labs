#pragma once

enum class ProcessMode {
    NORMAL,
    INVERT,
    CANNY,
    EXIT
};

class KeyProcessor {
private:
    ProcessMode currentMode;
public:
    KeyProcessor();
    void handleKey(int key);
    ProcessMode getCurrentMode() const;
};

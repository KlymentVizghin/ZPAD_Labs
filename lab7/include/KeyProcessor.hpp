#pragma once

enum class ProcessMode {
    NORMAL,
    INVERT,
    CANNY,
    FACE,
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

#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(ProcessMode::NORMAL) {}

void KeyProcessor::handleKey(int key) {
    switch (key) {
        case '1': currentMode = ProcessMode::NORMAL; break;
        case '2': currentMode = ProcessMode::INVERT; break;
        case '3': currentMode = ProcessMode::CANNY; break;
        case 'f':
        case 'F': currentMode = ProcessMode::FACE; break;
        case 27:  // ESC
        case 'q':
        case 'Q': currentMode = ProcessMode::EXIT; break;
        default: break;
    }
}

ProcessMode KeyProcessor::getCurrentMode() const {
    return currentMode;
}

#pragma once

namespace JzRE {

class JzPlugin {
public:
    /**
     * Execute the plugin behaviour
     */
    virtual void Execute() = 0;

    /* Feel free to store any data you want here */
    void *userData = nullptr;
};

} // namespace JzRE

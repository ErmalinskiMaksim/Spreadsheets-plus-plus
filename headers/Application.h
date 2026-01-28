#ifndef APPLICATION_H
#define APPLICATION_H

// Main application class
class Application {
public:
    // main loop 
    void run();
private:
    bool processEvents();
    void display();
};

#endif // !APPLICATION_H

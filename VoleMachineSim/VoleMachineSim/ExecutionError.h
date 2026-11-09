#ifndef EXECUTIONERROR_H
#define EXECUTIONERROR_H

#include <string>

class ExecutionError
{
private:
    std::string errorMessage;

public:
    ExecutionError();

    void setError(std::string message);
    std::string getError();
    void displayError();
    void clearError();
};

#endif

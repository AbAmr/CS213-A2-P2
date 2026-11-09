#include "ExecutionError.h"

#include <iostream>

ExecutionError::ExecutionError()
{
    errorMessage = "";
}

void ExecutionError::setError(std::string message)
{
    errorMessage = message;
}

std::string ExecutionError::getError()
{
    return errorMessage;
}

void ExecutionError::displayError()
{
    if (errorMessage != "")
    {
        std::cout << "Execution Error: " << errorMessage << std::endl;
    }
}

void ExecutionError::clearError()
{
    errorMessage = "";
}
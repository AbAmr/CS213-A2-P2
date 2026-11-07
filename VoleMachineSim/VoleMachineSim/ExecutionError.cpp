#include "ExecutionError.h"


ExecutionError::ExecutionError()
{
    errorMessage = "";
}

void ExecutionError::setError(string message)
{
    errorMessage = message;
}

string ExecutionError::getError()
{
    return errorMessage;
}

void ExecutionError::displayError()
{
    if (errorMessage != "")
    {
        cout << "Execution Error: " << errorMessage << endl;
    }
}

void ExecutionError::clearError()
{
    errorMessage = "";
}

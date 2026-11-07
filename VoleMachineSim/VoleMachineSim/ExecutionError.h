#pragma once

#ifndef EXECUTIONERROR_H
#define EXECUTIONERROR_H

#include <iostream>
#include <string>
using namespace std;

class ExecutionError
{
private:
    string errorMessage;

public:
    ExecutionError()
    {
        errorMessage = "";
    }

    void setError(string message)
    {
        errorMessage = message;
    }

    string getError()
    {
        return errorMessage;
    }

    void displayError()
    {
        if (errorMessage != "")
        {
            cout << "Execution Error: " << errorMessage << endl;
        }
    }

    void clearError()
    {
        errorMessage = "";
    }
};

#endif

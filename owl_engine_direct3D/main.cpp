#include "owl_window.h"

#include <sstream>

#include "application.h"

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    try
    {
        owl::application application;
        return application.start();
    }
    catch (const owl::exception& ex)
    {
        MessageBox(nullptr, ex.what(), ex.get_type(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& ex)
    {
        MessageBox(nullptr, ex.what(), "standard exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No details available", "unknown exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
}
@echo off
echo [System] Initializing LADOHD Insider Threat Detection Pipeline...

:: Launch the Python inference server in a new dedicated console window
start "LADOHD Inference Server" cmd /c "python server.py"

:: Provide a brief delay to ensure the TCP socket is actively listening
timeout /t 2 /nobreak >nul

:: Launch the compiled C++ ETW Agent with Administrator privileges via PowerShell
echo [System] Spawning kernel agent. Please accept the UAC prompt.
powershell -Command "Start-Process 'build\Debug\etw_agent.exe' -Verb RunAs"

echo [System] Pipeline operational.
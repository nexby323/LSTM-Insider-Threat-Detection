@echo off
echo [System] Initializing LADOHD Insider Threat Detection Pipeline...

:: Launch the Python inference server via Docker with a Bind Mount
:: This ensures event_vocab.txt and the model are saved directly to your host folder
start "LADOHD Inference Server" cmd /c "docker run -p 65432:65432 -v "%cd%:/app" ladohd-server"

:: Provide a brief delay to ensure the Docker container initializes 
timeout /t 5 /nobreak >nul

:: Launch the compiled C++ ETW Agent with Administrator privileges via PowerShell
echo [System] Spawning kernel agent. Please accept the UAC prompt.
powershell -Command "Start-Process 'build\Debug\etw_agent.exe' -Verb RunAs"

echo [System] Pipeline operational.
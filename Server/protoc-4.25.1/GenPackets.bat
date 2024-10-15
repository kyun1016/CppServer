protoc.exe -I=./ --cpp_out=./ ./Protocol.proto --experimental_editions
IF ERRORLEVEL 1 PAUSE
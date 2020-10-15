@echo.uninstall service...
@echo off  
@net stop Git客户端服务
@echo.delete service...
@sc delete Git客户端服务
@echo off  
@echo.delete success!
@pause
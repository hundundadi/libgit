@echo.install......  
@echo off   
@cd %~dp0
@sc create Git客户端服务 binPath= "%~dp0\GSP.exe"   
@sc config Git客户端服务 start= auto
@set base_dir=%~dp0  
@%base_dir:~0,2%  
@pushd %base_dir% 
@sc start Git客户端服务 9000
@popd	
@echo off  
@echo.install successed!  
@pause
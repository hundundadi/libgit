@echo.install......  
@echo off   
@cd %~dp0
@sc create Git�ͻ��˷��� binPath= "%~dp0\GSP.exe"   
@sc config Git�ͻ��˷��� start= auto
@set base_dir=%~dp0  
@%base_dir:~0,2%  
@pushd %base_dir% 
@sc start Git�ͻ��˷��� 9000
@popd	
@echo off  
@echo.install successed!  
@pause
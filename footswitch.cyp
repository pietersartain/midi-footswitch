<project buildenvironment="{5bb3321d-c620-467d-8787-1a268e2edf63}" executionenvironment="{ab5780d6-ed62-47a1-a50c-bc70aee3b715}" executionenvironmentname="eCOG1 Dev Board" version="1.00.0003" buildenvironmentname="eCOG1" >
 <folder name="ASM Files" >
  <extension name="asm" />
  <file path="${PROJECT_PATH}/pack.asm" />
  <file path="${PROJECT_PATH}/irq.asm" />
  <file path="${PROJECT_PATH}/cstartup.asm" />
 </folder>
 <folder name="C Files" >
  <extension name="c" />
  <file path="${PROJECT_PATH}/footswitch.c" />
  <file path="${PROJECT_PATH}/putchar.c" />
  <file path="${CYANIDE_INSTALLDIR}/examples/eCOG1k eval board/driver_lib/__assert.c" />
 </folder>
 <folder name="Header Files" >
  <extension name="h" />
 </folder>
 <file path="${PROJECT_PATH}/internal.map" />
 <file path="${PROJECT_PATH}/evalboard.cfg" />
 <properties currentconfiguration="Debug" >
  <configuration name="Debug" >
   <property key="brkInstallAddr" >0</property>
   <property key="defines" ></property>
   <property key="includeDirs" >${CYANIDE_INSTALLDIR}\examples\eCOG1k dev board\board_lib;${CYANIDE_INSTALLDIR}\libraries\eCOG1k;${CYANIDE_INSTALLDIR}\libraries\eCOG1k\Core</property>
   <property key="intermediateDir" >${PROJECT_PATH}/temp</property>
   <property key="libDirs" >${CYANIDE_INSTALLDIR}\examples\eCOG1k dev board\board_lib;${CYANIDE_INSTALLDIR}\libraries\eCOG1k\Core</property>
   <property key="mapFile" >${PROJECT_PATH}/internal</property>
   <property key="outputDir" >${PROJECT_PATH}/out</property>
   <property key="packFile" >${PROJECT_PATH}/pack</property>
   <property key="stabsInfo" >1</property>
  </configuration>
  <configuration name="Release" >
   <property key="brkInstallAddr" >0</property>
   <property key="defines" >NDEBUG</property>
   <property key="includeDirs" >${CYANIDE_INSTALLDIR}\examples\eCOG1k dev board\board_lib;${CYANIDE_INSTALLDIR}\libraries\eCOG1k;${CYANIDE_INSTALLDIR}\libraries\eCOG1k\Core</property>
   <property key="intermediateDir" >${PROJECT_PATH}/temp</property>
   <property key="libDirs" >${CYANIDE_INSTALLDIR}\examples\eCOG1k dev board\board_lib;${CYANIDE_INSTALLDIR}\libraries\eCOG1k\Core</property>
   <property key="mapFile" >${PROJECT_PATH}/internal</property>
   <property key="outputDir" >${PROJECT_PATH}/out</property>
   <property key="packFile" >${PROJECT_PATH}/pack</property>
   <property key="stabsInfo" >1</property>
  </configuration>
 </properties>
</project>

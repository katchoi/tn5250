000100101109/*-----------------------------------------------------------------*/           
000200101109/*  EW       Date        Developer  Flag  Description              */           
000300101109/*  EWK0002  11/09/2010  Kat        @K1   add subtree option       */           
000400101109/*-----------------------------------------------------------------*/           
000500070525#include <stdio.h>                                                              
000600070528#include <stdlib.h>                                                             
000700070525#include <string.h>                                                             
000800070616#include <errno.h>                                                              
000900070614#include <sys/types.h>                                                          
001000070614#include <sys/stat.h>                                                           
001100070528#include <quslobj.h>                                                            
001200070528                                                                                
001300070525#include <abdapputl.h>                                                          
001400070525#include <abddbutl.h>                                                           
001500070528#include <abdlutl.h>                                                            
001600070525                                                                                
001700100522int  processDAE_DAGs(char inTS[26], char inAnz[20]) ;                           
001800100522int  processDAE_DIRs(char inTS[26], char inAnz[20]) ;                           
001900100522int  processDAE_IFSs(char inTS[26], char inAnz[20]) ;                           
002000100523int  processDAE_DIR(char daeID[10], char filName[10], char dir[256],            
002100101110                    char inTS[26] , char inAnz[20], int lvl);                   
002200101109int  processDIR_IFSs(char daeID[10], char filName[10], char dir[256],           
002300110216                        char inTS[26], char inAnz[20], int *hasDir);            
002400100522                                                                                
002500100522int  processDirectories(char daeID[10], char inDag[20],                         
002600100522                        char inTS[26], char inAnz[20]);                         
002700100522int  processStmfs(char daeID[10],char inDag[20], char inTS[26], char inAnz[20]);
002800100522int  extractIfsInfo(char daeID[10],char inDag[20], char inTS[26],               
002900100522     char inAnz[20], char *path, char item[256], char filName[10]);             
003000100522int  extractIfsInfo1(char daeID[10], char inDag[20], char inTS[26],             
003100100522      char inAnz[20], char *dir, char *fullname, char filName[10]);             
003200100328int  extractDirectoryInfo(char inDag[20], char inTS[26], char inDdir[256]) ;    
003300100328int  extractDirectoryInfo1(char inDag[20], char inTS[26], char inDir[256]) ;    
003400070616int  getDirectories(char inDag[20], T_daeDir dirList[256]);                     
003500070616int  getExpandedDirectories(T_daeDir d, char *p_dirList, int *noDir);           
003600070618int  isDuplicated(char *item, T_daeDir dirList[256], int start, int end);       
003700100522int  applyFilter(char daeID[10], char inDag[20], char inTS[26], char inAnz[20]);
003800100524int  extractIfsInfo2(char daeID[10], char inDag[20], char inTS[26],             
003900100524                 char inAnz[20], char *dir, char *fullname, char filName[10]) ; 
004000100524int getExpandedStmfs(T_daeStmf inStmf, char *p_stmfList, int *noStmf) ;         
004100101109int  processDIR_SUBs(char daeID[10], char filName[10], char dir[256],           
004200101110                        char inTS[26], char inAnz[20], int lvl); /*@K1A*/       
004300070528                                                                                
004400110216                                                                                
004500101110                                                                                
004600070528int main(int argc, char *argv[])                                                
004700070525{                                                                               
004800070531   char dag[20];                                                                
004900070531   char runTS[26], endTS;                                                       
005000070606   long ifsCnt;                                                                 
005100070606   decimal(15,0) ifsSz;                                                         
005200070901   T_dags dags[512];                                                            
005300070901   int n, i;                                                                    
005400070525                                                                                
005500070525   memcpy(dag,   argv[1], sizeof(dag));                                         
005600070525   memcpy(runTS, argv[2], sizeof(runTS));                                       
005700100522   /* process all dae ifs objects for a given dag   */                          
005800100523   processDAE_IFSs(runTS, dag);                                                 
005900100522                                                                                
006000100522   /* process all dae directories for a given dag */                            
006100100523   processDAE_DIRs(runTS, dag);                                                 
006200100522                                                                                
006300100522   /* process all sub-dag for a given dag       */                              
006400100522   processDAE_DAGs(runTS, dag);                                                 
006500100522                                                                                
006600070606   getAnalysisIfsInfo(dag, runTS, &ifsCnt, &ifsSz);                             
006700070616   updateAnalysisIfsSummary(dag, runTS, ifsCnt, ifsSz);                         
006800070525   return 0;                                                                    
006900070525}                                                                               
007000070528                                                                                
007100100522/* process a DAG with the old processing logic and attaching */                 
007200100522/* the corresponding daeID                                   */                 
007300100522/*                                                           */                 
007400100522int processDAE_DAG(char daeID[10], char filName[10], char inDag[20],            
007500100522                                   char inTS[26], char inAnz[20])               
007600100522{                                                                               
007700100522   long ifsCnt;                                                                 
007800100522   decimal(15,0) ifsSz;                                                         
007900100522   T_dags dags[512];                                                            
008000100522   int n, i;                                                                    
008100100522                                                                                
008200100522   n = getDAGs(inDag, dags);                                                    
008300100522   for ( i=0; i<n; i++ ) {                                                      
008400100522      processStmfs(daeID, dags[i].dag, inTS, inAnz);                            
008500100522      processDirectories(daeID, dags[i].dag, inTS, inAnz);                      
008600100522      applyFilter(daeID, dags[i].dag, inTS, inAnz);                             
008700100522   }                                                                            
008800100522   return 0;                                                                    
008900100522}                                                                               
009000100522                                                                                
009100100522/* extract all sub-DAGs (dae) and process process them one at a time */         
009200100522int  processDAE_DAGs(char inTS[26], char inAnz[20])                             
009300100522{                                                                               
009400100522   T_daedags subs[512];                                                         
009500100522   int i, cnt = 0;                                                              
009600100522                                                                                
009700100522   cnt = getDAE_DAG(inAnz, (char *)&subs, 0);                                   
009800100522   for ( i=0; i<cnt; i++ ) {                                                    
009900100522      processDAE_DAG(subs[i].daeid, subs[i].filName, subs[i].dag,               
010000100522                                                            inTS, inAnz);       
010100100522   }                                                                            
010200100522   return cnt;                                                                  
010300100522}                                                                               
010400100522                                                                                
010500100523int  processDAE_IFSs(char inTS[26], char inAnz[20])                             
010600100523{                                                                               
010700100602   T_daeStmf *p_stmfs=NULL, *p;                                                 
010800100523   int i, noObjs;                                                               
010900100523   char   fullname[257];                                                        
011000100523                                                                                
011100100523   noObjs = getDAEStmfCount(inAnz);                                             
011200100523   p_stmfs = malloc(noObjs * sizeof(T_daeObject));                              
011300100523   getDAEStmfs(inAnz, (char *)p_stmfs);                                         
011400100523   p = p_stmfs;                                                                 
011500100523   for ( i=0; i<noObjs; i++ ) {                                                 
011600100523      nonBlankCpy(fullname, p->dae, 256); /*fixme*/                             
011700100523      extractIfsInfo1(p->daeid, inAnz, inTS, inAnz, "", fullname, p->filName);  
011800100523      p++;                                                                      
011900100523   }                                                                            
012000100523   if ( p_stmfs != NULL )                                                       
012100100523      free(p_stmfs);                                                            
012200100523   return 0;                                                                    
012300100523}                                                                               
012400100523                                                                                
012500100523int  processDAE_DIRs(char inTS[26], char inAnz[20])                             
012600100523{                                                                               
012700100607   int i, nodir, rc, pos = 0;                                                   
012800100605   T_daeDir  dirs[1024];                                                        
012900100523   char temp[257], cmd[256];                                                    
013000100523                                                                                
013100100523   nodir = getDAEDirectories(inAnz, (char *)dirs);                              
013200100523   for (i=0; i<nodir; i++) {                                                    
013300100523      /* for each dae directory entry, process it. Expand */                    
013400100523      /* it if needed                                     */                    
013500101110      processDAE_DIR(dirs[i].daeid, dirs[i].filName,                            
013600101110                                    dirs[i].dir, inTS, inAnz, 0);               
013700100523   }                                                                            
013800100523   return nodir;                                                                
013900100523}                                                                               
014000100523                                                                                
014100101109int  isSubtree(char daeID[10])                                                  
014200101109{                                                                               
014300101109   char option[10];                                                             
014400101109                                                                                
014500101109   int i = getDAEOption(daeID, "*SUBTREE  ", option);                           
014600101109   if ( i == 0 )                                                                
014700101109      return memcmp(option, "*NONE", 5);                                        
014800101109   return 0;                                                                    
014900101109}                                                                               
015000101109                                                                                
015100100523int  processDAE_DIR(char daeID[10], char filName[10], char dir[256],            
015200101110                    char inTS[26] , char inAnz[20], int lvl)                    
015300100523{                                                                               
015400100523   T_daeDir  dirs[1024];                                                        
015500101109   char temp[257];                                                              
015600100523   int  n, i;                                                                   
015700110216   int  hasDir ;                                                                
015800101109   int b = isSubtree(daeID);                                                    
015900101109                                                                                
016000100603   memset(&dirs, ' ', 1024*sizeof(T_daeDir));                                   
016100100523   n = getDirs(dir, dirs);                                                      
016200100523   for ( i=0; i<n; i++ ) {                                                      
016300110216      hasDir = 0;                                                               
016400110216      processDIR_IFSs(daeID, filName, dirs[i].dir,  inTS, inAnz, &hasDir);      
016500110216      nonBlankCpy(temp, (char *) dirs[i].dir, 256);                             
016600110216      if (b && hasDir) {        /*@K1A*/                                        
016700110216         if (isDebug())                                                         
016800101110            printf("include subtree for directory: <%s> \n", temp);             
016900101110         processDIR_SUBs(daeID, filName,  dirs[i].dir, inTS, inAnz, lvl);       
017000110216      } else                                                                    
017100110216         if (isDebug())                                                         
017200110216            printf("exclude subtree for directory: <%s> \n", temp);             
017300100523   }                                                                            
017400100523}                                                                               
017500100523                                                                                
017600101109int  processDIR_IFSs(char daeID[10], char filName[10], char dir[256],           
017700110216                     char inTS[26], char inAnz[20], int *hasDir)                
017800100523{                                                                               
017900100523   char wanted[128] = "*STMF,*PGM,*MODULE";                                     
018000100523   int  ndir, nentry, i, j;                                                     
018100100523   T_daeDir  dirList[256];                                                      
018200100523   char      *p_entry, temp[257];                                               
018300100523   T_IFS     *p;                                                                
018400100523                                                                                
018500100523   nonBlankCpy(temp, dir, 256);                                                 
018600100523   /* extract *STMF only */                                                     
018700110216   nentry = listIFSs(temp, NULL, wanted, &p_entry, hasDir);                     
018800100523   p = (T_IFS *)p_entry;                                                        
018900100523   for (j=0; j<nentry; j++) {                                                   
019000100523      extractIfsInfo(daeID, inAnz, inTS, inAnz,                                 
019100100523                                  temp, p->ifsName, filName);                   
019200110216      p = p->next;                                                              
019300100523   }                                                                            
019400100523   extractDirectoryInfo(inAnz, inTS, dir);                                      
019500100523   return 0;                                                                    
019600100523}                                                                               
019700100523                                                                                
019800101109int  processDIR_SUBs(char daeID[10], char filName[10], char dir[256],           
019900101110                     char inTS[26], char inAnz[20], int lvl)  /*@K1A*/          
020000101109{                                                                               
020100101109   char *list;                                                                  
020200110216   T_IFS st[4096];                                                              
020300101109   T_IFS *p;                                                                    
020400101109   char temp[267];                                                              
020500101110   int i, n, j, nextLvl = lvl+1;                                                
020600110216   int       hasDir = 0;                                                        
020700101109                                                                                
020800101109   nonBlankCpy(temp, dir, 256);                                                 
020900110216   n = listIFSs(temp, "*", "*DIR", &list, &hasDir);                             
021000101110   p = (T_IFS *) list;                                                          
021100101110   for (i=0; i<n; i++) {                                                        
021200101110       st[i] = *p;                                                              
021300110216       p = p->next;                                                             
021400101110   }                                                                            
021500101109   for (i=0; i<n; i++) {                                                        
021600101110      nonBlankCpy(temp, st[i].ifsName, 256);                                    
021700110216      if (isDebug())                                                            
021800101110      printf("<%d>processDIR_SUBs<%d>dir: <%s> level <%d>\n", n, i, temp, lvl); 
021900101110      processDAE_DIR(daeID, filName, st[i].ifsName, inTS , inAnz, nextLvl);     
022000101109   }                                                                            
022100101109   return n;                                                                    
022200101109}                                                                               
022300101109                                                                                
022400100522int  applyFilter(char daeID[10], char inDag[20], char inTS[26], char inAnz[20]) 
022500070604{                                                                               
022600070604   return 0;                                                                    
022700070604}                                                                               
022800100523                                                                                
022900100522int  processDirectories(char daeID[10], char inDag[20],                         
023000100522                        char inTS[26], char inAnz[20])                          
023100070531{                                                                               
023200080605   char wanted[128] = "*STMF,*PGM,*MODULE";                                     
023300070616   int  ndir, nentry, i, j;                                                     
023400100605   T_daeDir  dirList[1024];                                                     
023500070616   char      *p_entry, temp[257];                                               
023600070614   T_IFS     *p;                                                                
023700110216   int       hasDir = 0;                                                        
023800070531                                                                                
023900100605   memset(&dirList, ' ', 1024*sizeof(T_daeDir));                                
024000070614   ndir = getDirectories(inDag, dirList);                                       
024100070616   for ( i=0; i<ndir; i++ ) {                                                   
024200070618      nonBlankCpy(temp, dirList[i].dir, 256);                                   
024300070618      /* if duplicate, skip */                                                  
024400070618      if ( isDuplicated(temp, dirList, i+1, ndir) )                             
024500070618         continue;                                                              
024600070616      /* extract *STMF only */                                                  
024700110216      nentry = listIFSs(temp, NULL, wanted, &p_entry, &hasDir);                 
024800070616      p = (T_IFS *)p_entry;                                                     
024900070614      for (j=0; j<nentry; j++) {                                                
025000100522         extractIfsInfo(daeID, inDag, inTS, inAnz,                              
025100070823                                     temp, p->ifsName, dirList[i].filName);     
025200110216         p = p->next;                                                           
025300070614      }                                                                         
025400100328      extractDirectoryInfo(inAnz, inTS, dirList[i].dir);                        
025500070614   }                                                                            
025600070614   return 0;                                                                    
025700070531}                                                                               
025800070531                                                                                
025900100522int  processStmfs(char daeID[10], char inDag[20], char inTS[26], char inAnz[20])
026000070905{                                                                               
026100070905   T_daeObject *p_stmfs=NULL, *p;                                               
026200070905   int i, noObjs;                                                               
026300070905   char   fullname[257];                                                        
026400070905                                                                                
026500070905   noObjs = getDAEStmfCount(inDag);                                             
026600070905   p_stmfs = malloc(noObjs * sizeof(T_daeObject));                              
026700070905   getDAEStmfs(inDag, (char *)p_stmfs);                                         
026800070906   p = p_stmfs;                                                                 
026900070905   for ( i=0; i<noObjs; i++ ) {                                                 
027000100328      nonBlankCpy(fullname, p->dae, 256); /*fixme*/                             
027100100522      extractIfsInfo1(daeID, inDag, inTS, inAnz, "", fullname, p->filName);     
027200070905      p++;                                                                      
027300070905   }                                                                            
027400070905}                                                                               
027500070905                                                                                
027600070618int isDuplicated(char *item, T_daeDir dirList[256], int start, int end)         
027700070618{                                                                               
027800070618   char temp[257];                                                              
027900070618   int  i;                                                                      
028000070618                                                                                
028100070618   for ( i=start; i<end; i++ ) {                                                
028200070618      nonBlankCpy(temp, dirList[i].dir, 256);                                   
028300070618      if ( strcmp(temp, item) == 0 )                                            
028400070618         return 1;                                                              
028500070618   }                                                                            
028600070618   return 0;                                                                    
028700070618}                                                                               
028800070618                                                                                
028900100524int  extractIfsInfo2(char daeID[10], char inDag[20], char inTS[26],             
029000100524                    char inAnz[20], char *dir, char *fullname, char filName[10])
029100100524{                                                                               
029200100524   struct stat   s_stat;                                                        
029300100608   char parent[257], child[257];                                                
029400100524   char   owner[10];                                                            
029500100524   char   crtDate[26];                                                          
029600100524   char   chgDate[26];                                                          
029700100524   char   useDate[26];                                                          
029800100524   char   type[7];                                                              
029900100524   char   size[16];                                                             
030000100602   char   temp[257];                                                            
030100100524   int    i = 0;                                                                
030200100524                                                                                
030300100602   nonBlankCpy(temp, fullname, 256);                                            
030400100602   if (stat(temp, &s_stat)) {                                                   
030500100603      printf("1 Something worng with %s error code:%d\n",temp, errno)  ;        
030600100524      return 1;                                                                 
030700100524   }                                                                            
030800110123   i = s_stat.st_size;                                                          
030900110216   if (isDebug())                                                               
031000110123      printf("=====> processing: %s <%d> daeID: %10.10s\n", temp, i, daeID);    
031100100524   memcpy(type, s_stat.st_objtype, 7);                                          
031200100524   sprintf(size, "%015d", i);                                                   
031300100524   tmToSQLTimestamp(gmtime(&(s_stat.st_ctime)), crtDate);                       
031400100524   tmToSQLTimestamp(gmtime(&(s_stat.st_mtime)), chgDate);                       
031500100524   tmToSQLTimestamp(gmtime(&(s_stat.st_atime)), useDate);                       
031600100605   getIFSOwner(temp, owner);                                                    
031700100608                                                                                
031800100608   decomposePath(temp, parent, child);                                          
031900110225/* if ( checkFilter(child, type, owner, "   ", filName) == 0 )         {        
032000100608      printf("filter exclude item: %s\n", child);                               
032100100605      return 1;                                                                 
032200110225   } */                                                                         
032300100524   return addAnalysisIfsRecord(inAnz, inTS, daeID, inDag, dir, fullname, type,  
032400100524                     filName, owner, size, crtDate, chgDate, useDate);          
032500100524}                                                                               
032600100524                                                                                
032700100522int  extractIfsInfo1(char daeID[10], char inDag[20], char inTS[26],             
032800100522                    char inAnz[20], char *dir, char *fullname, char filName[10])
032900070614{                                                                               
033000100524   T_daeStmf stmf, stmfList[4096];                                              
033100100524   int    i = 0, pos = 0;                                                       
033200070614                                                                                
033300100603   memset(&stmf, ' ', sizeof(T_daeStmf));                                       
033400100524   memcpy(stmf.dae, fullname, strlen(fullname));                                
033500100524   memcpy(stmf.daeid, daeID, 10);                                               
033600100524   if ( strstr(fullname, "*") != NULL ) {                                       
033700100524      getExpandedStmfs(stmf, (char *)&stmfList[pos], &pos);                     
033800100602                                                                                
033900100602   } else {                                                                     
034000100602      stmfList[0] = stmf;                                                       
034100100602      pos++;                                                                    
034200100602                                                                                
034300100602   }                                                                            
034400100524   for( i=0; i<pos; i++ ) {                                                     
034500100524      extractIfsInfo2(daeID, inDag, inTS, inAnz,                                
034600100524                                 dir, stmfList[i].dae, filName);                
034700100524                                                                                
034800100524   }                                                                            
034900070614}                                                                               
035000070905                                                                                
035100100522int  extractIfsInfo(char daeID[1], char inDag[20], char inTS[26],               
035200100522         char inAnz[20], char *path, char item[256], char filName[10])          
035300070905{                                                                               
035400070905   struct stat   s_stat;                                                        
035500070905   char   owner[10];                                                            
035600070905   char   crtDate[26];                                                          
035700070905   char   chgDate[26];                                                          
035800070905   char   useDate[26];                                                          
035900070905   char   temp[257];                                                            
036000070905   char   fullname[257];                                                        
036100070905   char   size[16];                                                             
036200070905                                                                                
036300070905   nonBlankCpy(temp, item, 256);                                                
036400100522   return extractIfsInfo1(daeID, inDag, inTS, inAnz,                            
036500100603                                        path, temp, filName);                   
036600070905}                                                                               
036700070614                                                                                
036800100328int  extractDirectoryInfo(char inDag[20], char inTS[26], char inDir[256])       
036900100328{                                                                               
037000100328   struct stat   s_stat;                                                        
037100100328   char   temp[257];                                                            
037200100328   char   fullname[257];                                                        
037300100328   char   size[16];                                                             
037400100328                                                                                
037500100328   extractDirectoryInfo1(inDag, inTS, inDir);                                   
037600100328   return 0;                                                                    
037700100328}                                                                               
037800100328                                                                                
037900100328int  extractDirectoryInfo1(char inDag[20], char inTS[26], char inDir[256])      
038000100328{                                                                               
038100100328   struct stat   s_stat;                                                        
038200100328   char   temp[256], dir[257];                                                  
038300100328   char   owner[10];                                                            
038400100328   char   crtDate[26];                                                          
038500100328   char   chgDate[26];                                                          
038600100328   char   useDate[26];                                                          
038700100328   char   type[7];                                                              
038800100328   char   size[16];                                                             
038900100328   int    i = 0;                                                                
039000100328                                                                                
039100100328   memset(temp, ' ', 256);                                                      
039200100328   memcpy(temp, inDir, strlen(inDir));                                          
039300100328   nonBlankCpy(dir, inDir, 256);                                                
039400100328   if (stat(dir, &s_stat)) {                                                    
039500100603      printf("2 Something worng with %s error code:%d\n",dir, errno)   ;        
039600100328      return 1;                                                                 
039700100328   }                                                                            
039800100328   tmToSQLTimestamp(gmtime(&(s_stat.st_ctime)), crtDate);                       
039900100328   tmToSQLTimestamp(gmtime(&(s_stat.st_mtime)), chgDate);                       
040000100328   tmToSQLTimestamp(gmtime(&(s_stat.st_atime)), useDate);                       
040100100328   getIFSOwner(dir, owner);                                                     
040200100328   return addAnalysisDirRecord(inDag, inTS, temp,                               
040300100328                     owner, crtDate, chgDate, useDate);                         
040400100328}                                                                               
040500100328                                                                                
040600100603int getDirs(char inDir[256], char *list)                                        
040700100523{                                                                               
040800100607   int i, nodir, rc, pos = 0;                                                   
040900100603   T_daeDir  dirs[1], *p;                                                       
041000100523   char temp[257], cmd[256];                                                    
041100100523                                                                                
041200100603   memset(&dirs, ' ', sizeof(T_daeDir));                                        
041300100603   memcpy(dirs[0].dir, inDir, 256);                                             
041400100603   nonBlankCpy(temp, inDir, 256);                                               
041500100603   if ( strstr(temp, "*") != NULL )                                             
041600100603      getExpandedDirectories(dirs[0], (char *)list, &pos);                      
041700100603   else {                                                                       
041800100603      p = (T_daeDir *)list;                                                     
041900100603      memcpy(p->dir, inDir, 256);                                               
042000100603      pos++;                                                                    
042100100603   }                                                                            
042200100523   return pos;                                                                  
042300100523}                                                                               
042400100523                                                                                
042500100328                                                                                
042600100605int getDirectories(char inDag[20], T_daeDir dirList[1024])                      
042700070528{                                                                               
042800100607   int i, nodir, rc, pos = 0;                                                   
042900070613   T_daeDir  dirs[256];                                                         
043000070613   char temp[257], cmd[256];                                                    
043100070528                                                                                
043200070613   nodir = getDAEDirectories(inDag, (char *)dirs);                              
043300070613   for (i=0; i<nodir; i++) {                                                    
043400070613      nonBlankCpy(temp, dirs[i].dir, 256);                                      
043500070528      if ( strstr(temp, "*") != NULL )                                          
043600100605         getExpandedDirectories(dirs[i], (char *)dirList[pos].dir, &pos);       
043700070528      else {                                                                    
043800070613         memcpy(dirList[pos].dir, dirs[i].dir, sizeof(T_daeDir));               
043900070528         pos++;                                                                 
044000070528      }                                                                         
044100070528   }                                                                            
044200070529   return pos;                                                                  
044300070528}                                                                               
044400070613                                                                                
044500070616int getExpandedDirectories(T_daeDir indir, char *p_dirList, int *noDir )        
044600070613{                                                                               
044700070616   int inDir, i, n = 0;                                                         
044800070615   char temp[257], *q;                                                          
044900100603   char parent[257], child[257], *list;                                         
045000100603   T_IFS *p;                                                                    
045100070615   T_daeDir *d;                                                                 
045200110216   int       hasDir = 0;                                                        
045300070614                                                                                
045400100603   inDir = *noDir;                                                              
045500070616   nonBlankCpy(temp, indir.dir, 256);                                           
045600100607/* q = strtok(temp, "*");    */                                                 
045700100607   decomposePath(temp, parent, child);                                          
045800070618   /* select item prefix with value = q */                                      
045900110216   n = listIFSs(parent, child, "*DIR", &list, &hasDir);                         
046000070615   p = (T_IFS *) list;                                                          
046100070616   d = (T_daeDir *)p_dirList;                                                   
046200070618/* d += *noDir;      */                                                         
046300070614   for (i=0; i<n; i++) {                                                        
046400100603      memset(d, ' ', sizeof(T_daeDir));                                         
046500100524      memcpy(d->filName, indir.filName, 10);                                    
046600100603      memcpy(d->dir, p->ifsName, 256);                                          
046700110216      p = p->next; d++;                                                         
046800070614   }                                                                            
046900100603   inDir += n;                                                                  
047000100603   *noDir = inDir;                                                              
047100070616   return n;                                                                    
047200070613}                                                                               
047300070613                                                                                
047400100524int getExpandedStmfs(T_daeStmf inStmf, char *p_stmfList, int *noStmf)           
047500100524{                                                                               
047600100608   int i, n = 0, no = 0;                                                        
047700100524   char temp[257], *q;                                                          
047800100524   char parent[257], child[257], *list;                                         
047900100524   T_IFS *p;                                                                    
048000100524   T_daeStmf *d;                                                                
048100110216   int       hasDir = 0;                                                        
048200100524                                                                                
048300100608   no = *noStmf;                                                                
048400100524   nonBlankCpy(temp, inStmf.dae, 256);                                          
048500100608/* q = strtok(temp, "*");       */                                              
048600100608   decomposePath(temp, parent, child);                                          
048700100524   /* select item prefix with value = q */                                      
048800110216   n = listIFSs(parent, child, "*STMF", &list, &hasDir);                        
048900100524   p = (T_IFS *) list;                                                          
049000100524   d = (T_daeStmf *)p_stmfList;                                                 
049100100524/* d += *noDir;      */                                                         
049200100524   for (i=0; i<n; i++) {                                                        
049300100603      memset(d, ' ', sizeof(T_daeStmf));                                        
049400100524      memcpy(d->filName, inStmf.filName, 10);                                   
049500100524      memcpy(d->path, parent, strlen(parent));                                  
049600100603      memcpy(d->dae, p->ifsName, sizeof(d->dae));                               
049700100524      memcpy(d->daeid, inStmf.daeid, sizeof(d->daeid));                         
049800110216      p = p->next; d++;                                                         
049900100524   }                                                                            
050000100608   no += n;                                                                     
050100100608   *noStmf = no;                                                                
050200100524   return n;                                                                    
050300100524}                                                                               
050400100524                                                                                
050500070528                                                                                

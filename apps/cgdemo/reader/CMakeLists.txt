set(source_files
  AbstractParser.cpp
  Buffer.h
  Expression.cpp  
  FileBuffer.h    
  ReaderBase.h     
  Scope.cpp
  AbstractParser.h    
  ErrorHandler.cpp  
  Expression.h    
  SceneReader.cpp  
  Scope.h
  Buffer.cpp 
  ErrorHandler.h
  FileBuffer.cpp  
  ReaderBase.cpp  
  SceneReader.h    
  StringRef.h
  )

add_library(ds-cg-util STATIC ${source_files})
target_include_directories(ds-cg-util PUBLIC  ../../../cg/include)
target_compile_features(ds-cg-util PUBLIC cxx_std_20)

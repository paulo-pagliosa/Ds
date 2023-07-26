set(source_files
  reader/AbstractParser.cpp
  reader/Buffer.h
  reader/Expression.cpp  
  reader/FileBuffer.h    
  reader/ReaderBase.h     
  reader/Scope.cpp
  reader/AbstractParser.h    
  reader/ErrorHandler.cpp  
  reader/Expression.h    
  reader/SceneReader.cpp  
  reader/Scope.h
  reader/Buffer.cpp 
  reader/ErrorHandler.h
  reader/FileBuffer.cpp  
  reader/ReaderBase.cpp  
  reader/SceneReader.h    
  reader/StringRef.h
  RayTracer.h
  RayTracer.cpp
  SceneWriter.h
  SceneWriter.cpp
  Writer.h
  Writer.cpp)

add_library(ds-cg-util STATIC ${source_files})
target_include_directories(ds-cg-util PUBLIC  ../../cg/include)
target_compile_features(ds-cg-util PUBLIC cxx_std_20)

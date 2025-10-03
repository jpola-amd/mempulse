# cmake_minimum_required(VERSION 3.19)
include(CheckSourceCompiles)

function(check_std_expected OUT_VAR)
  # OUT_VAR will be set to 1 if std::expected compiles, else 0
  # Additionally, OUT_VAR_CXXFLAG will contain the flag used (may be empty).

  set(_code [[
    #include <expected>
    int use_it() {
      std::expected<int,int> e{42};
      return e ? *e : 0;
    }
    int main() { return use_it(); }
  ]])

  # Try a few likely flags; empty string means "use current defaults"
  set(_flag_candidates "" "-std=c++23" "-std=c++2b" "/std:c++23" "/std:c++latest")

  # Preserve and restore CMAKE_REQUIRED_FLAGS
  set(_old_req_flags "${CMAKE_REQUIRED_FLAGS}")

  set(_found 0)
  set(_used_flag "")

  foreach(_f IN LISTS _flag_candidates)
    set(CMAKE_REQUIRED_FLAGS "${_old_req_flags} ${_f}")
    check_source_compiles(CXX "${_code}" _compiles)
    if(_compiles)
      set(_found 1)
      set(_used_flag "${_f}")
      break()
    endif()
  endforeach()

  # Restore original required flags
  set(CMAKE_REQUIRED_FLAGS "${_old_req_flags}")

  # Export results to caller scope
  set(${OUT_VAR} ${_found} PARENT_SCOPE)
  set(${OUT_VAR}_CXXFLAG "${_used_flag}" PARENT_SCOPE)
endfunction()

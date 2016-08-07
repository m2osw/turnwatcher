include( FindBoost )
find_package( Boost 1.50 COMPONENTS serialization ) #unit_test_framework )

include_directories( ${Boost_INCLUDE_DIR} )

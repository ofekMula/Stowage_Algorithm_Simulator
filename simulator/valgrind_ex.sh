rm -rf ../outputs/*
valgrind-3.14.0 --leak-check=full --show-leak-kinds=all ./simulator -travel_path ../all_travels -algorithm_path ../algorithm -output ../outputs -num_threads 50
redis-cli -h 10.112.179.21 -p 6379 -n 0 keys "*" | xargs redis-cli -h 10.112.179.21 -n 0 del


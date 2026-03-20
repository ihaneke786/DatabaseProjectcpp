To compile
cd src
c++ -std=c++17 -Wall -Wextra main.cpp repl.cpp statement.cpp table.cpp row.cpp pager.cpp -I../include -o db

or
cd src
g++ -std=c++17 -Wall -Wextra main.cpp repl.cpp statement.cpp table.cpp row.cpp pager.cpp -I../include -o db

To run
cd src
./db

Quick test
cd src
rm -f test.db
printf "insert 1 user1 email1\ninsert 2 user2 email2\nselect\n.exit\n" | ./db

Expected output
1 user1 email1
2 user2 email2

In this Project, we implemented a simplified version of an HTTP server

What it does:
  - It reads and parse its configuration file
  - It reads and parses entering HTTP requests
  - it executes CGI scripts if needed
  - it sends apropriate HTTP Responses

constraints:
  -use select, a new connection is handled through a thread or a process
  -never read or write on a file descriptor before going through select
  -only one read each time we go through select

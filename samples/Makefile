shared_mem: clean
	gcc shared_memory.cpp -I/usr/local/include/ -L/usr/local/lib -lrt -o shared_mem

named_pipe: clean
	gcc named_pipe.cpp -I/usr/local/include/ -L/usr/local/lib -o named_pipe

unix_domain_socket: clean
	gcc unix_domain_socket.cpp -I/usr/local/include/ -L/usr/local/lib -o unix_domain_socket

clean:
	rm -rf shared_mem named_pipe unix_domain_socket


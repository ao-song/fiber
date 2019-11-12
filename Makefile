lib:
	gcc -shared -fPIC -o libfiber.so ./*.c
clean:
	rm ./*.so

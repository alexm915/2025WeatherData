all: demo1 demo2 demo10 demo12 demo14 demo16 demo18 demo20 demo22 demo23\
	demo24 demo26 demo28 demo29 demo30 demo32 demo34 demo36\
	demo37 demo38 demo39 demo42 demo44 demo45 demo46 demo47 demo50 demo51 demo52

demo1:demo1.cpp
	g++ -g -o demo1 demo1.cpp ../_public.cpp 
	mv -f demo1 ./bin/.

demo2:demo2.cpp
	g++ -g -o demo2 demo2.cpp ../_public.cpp 
	mv -f demo2 ./bin/.

demo10:demo10.cpp
	g++ -g -o demo10 demo10.cpp ../_public.cpp  
	mv -f demo10 ./bin/.

demo12:demo12.cpp
	g++ -g -o demo12 demo12.cpp ../_public.cpp  
	mv -f demo12 ./bin/.

demo16:demo16.cpp
	g++ -g -o demo16 demo16.cpp ../_public.cpp  
	mv -f demo16 ./bin/.

demo18:demo18.cpp
	g++ -g -o demo18 demo18.cpp ../_public.cpp  
	mv -f demo18 ./bin/.

demo20:demo20.cpp
	g++ -g -o demo20 demo20.cpp ../_public.cpp  
	mv -f demo20 ./bin/.

demo14:demo14.cpp
	g++ -g -o demo14 demo14.cpp ../_public.cpp  
	mv -f demo14 ./bin/.

demo22:demo22.cpp
	g++ -g -o demo22 demo22.cpp ../_public.cpp  
	mv -f demo22 ./bin/.

demo23:demo23.cpp
	g++ -g -o demo23 demo23.cpp ../_public.cpp  
	mv -f demo23 ./bin/.

demo24:demo24.cpp
	g++ -g -o demo24 demo24.cpp ../_public.cpp  
	mv -f demo24 ./bin/.

demo26:demo26.cpp
	g++ -g -o demo26 demo26.cpp ../_public.cpp  
	mv -f demo26 ./bin/.

demo28:demo28.cpp
	g++ -g -o demo28 demo28.cpp ../_public.cpp  
	mv -f demo28 ./bin/.

demo29:demo29.cpp
	g++ -g -o demo29 demo29.cpp ../_public.cpp  
	mv -f demo29 ./bin/.

demo30:demo30.cpp
	g++ -g -o demo30 demo30.cpp ../_public.cpp  
	mv -f demo30 ./bin/.

demo32:demo32.cpp
	g++ -g -o demo32 demo32.cpp ../_public.cpp  
	mv -f demo32 ./bin/.

demo34:demo34.cpp
	g++ -g -o demo34 demo34.cpp ../_public.cpp  
	mv -f demo34 ./bin/.

demo36:demo36.cpp
	g++ -g -o demo36 demo36.cpp ../_public.cpp  
	mv -f demo36 ./bin/.

demo37:demo37.cpp
	g++ -g -o demo37 demo37.cpp ../_public.cpp  
	mv -f demo37 ./bin/.

demo38:demo38.cpp
	g++ -g -o demo38 demo38.cpp ../_public.cpp  
	mv -f demo38 ./bin/.

demo39:demo39.cpp
	g++ -g -o demo39 demo39.cpp ../_public.cpp  
	mv -f demo39 ./bin/.

demo42:demo42.cpp
	g++ -g -o demo42 demo42.cpp ../_public.cpp  -lpthread
	mv -f demo42 ./bin/.

demo44:demo44.cpp
	g++ -g -o demo44 demo44.cpp ../_public.cpp  
	mv -f demo44 ./bin/.

demo45:demo45.cpp
	g++ -g -o demo45 demo45.cpp ../_public.cpp  
	mv -f demo45 ./bin/.

demo46:demo46.cpp
	g++ -g -o demo46 demo46.cpp ../_public.cpp  
	mv -f demo46 ./bin/.

demo47:demo47.cpp
	g++ -g -o demo47 demo47.cpp ../_public.cpp  
	mv -f demo47 ./bin/.

demo48:demo48.cpp
	g++ -g -o demo48 demo48.cpp ../_public.cpp -lpthread  
	mv -f demo48 ./bin/.

demo50:demo50.cpp
	cd .. && gcc -c -o libftp.a ftplib.c  
	cd .. && gcc -fPIC -shared -o libftp.so ftplib.c  
	g++ -g -o demo50 demo50.cpp ../_public.cpp ../_ftp.cpp ../libftp.a 
	mv -f demo50 ./bin/.

demo51:demo51.cpp
	g++ -g -o demo51 demo51.cpp ../_public.cpp ../_ftp.cpp ../libftp.a 
	mv -f demo51 ./bin/.

demo52:demo52.cpp
	g++ -g -o demo52 demo52.cpp ../_public.cpp ../_ftp.cpp ../libftp.a
	mv -f demo52 ./bin/.

clean:
	rm -rf ./bin/*
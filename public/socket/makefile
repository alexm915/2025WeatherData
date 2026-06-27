all:client tcpselect tcppoll tcpepoll client1 tcpepoll1 tcpepoll2 client2 client3 client4\
	select

select:select.cpp
	g++ select.cpp -o -g select ../_public.cpp -lm -lc
	mv -f select ./bin/.
client1:client1.cpp
	g++ -g -o client1 client1.cpp -lm -lc
	mv -f client1 ./bin/.
client2:client2.cpp
	g++ -g -o client2 client2.cpp -lm -lc
	mv -f client2 ./bin/.
client3:client3.cpp
	g++ -g -o client3 client3.cpp -lm -lc
	mv -f client3 ./bin/.
client4:client4.cpp
	g++ -g -o client4 client4.cpp -lm -lc
	mv -f client4 ./bin/.
client:client.cpp
	g++ -g -o client client.cpp ../_public.cpp -lm -lc
	mv -f client ./bin/.
tcpselect:tcpselect.cpp
	g++ -g -o tcpselect tcpselect.cpp ../_public.cpp -lm -lc
	mv -f tcpselect ./bin/.
tcppoll:tcppoll.cpp
	g++ -g -o tcppoll tcppoll.cpp ../_public.cpp -lm -lc
	mv -f tcppoll ./bin/.
tcpepoll:tcpepoll.cpp
	g++ -g -o tcpepoll tcpepoll.cpp ../_public.cpp -lm -lc
	mv -f tcpepoll ./bin/.
tcpepoll1:tcpepoll1.cpp
	g++ -g -o tcpepoll1 tcpepoll1.cpp ../_public.cpp -lm -lc
	mv -f tcpepoll1 ./bin/.
tcpepoll2:tcpepoll2.cpp
	g++ -g -o tcpepoll2 tcpepoll2.cpp ../_public.cpp -lm -lc
	mv -f tcpepoll2 ./bin/.

clean:
	rm -rf ./bin/*
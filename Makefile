DIR = out

bataille : reset server client

server : server.o game.o shared_memory.o network.o utils.o
	cc -o $(DIR)/$@ $(DIR)/server.o $(DIR)/game.o $(DIR)/shared_memory.o $(DIR)/network.o $(DIR)/utils.o

client : client.o card.o shared_memory.o network.o utils.o
	cc -o $(DIR)/$@ $(DIR)/client.o $(DIR)/card.o $(DIR)/shared_memory.o $(DIR)/network.o $(DIR)/utils.o

server.o : server.c
	cc -c server.c -o $(DIR)/$@
client.o : client.c
	cc -c client.c -o $(DIR)/$@
game.o : game.c
	cc -c game.c -o $(DIR)/$@
shared_memory.o : shared_memory.c
	cc -c shared_memory.c -o $(DIR)/$@
network.o : network.c
	cc -c network.c -o $(DIR)/$@
card.o : card.c
	cc -c card.c -o $(DIR)/$@
utils.o : utils.c
	cc -c utils.c -o $(DIR)/$@

.PHONY : clean mkdir reset

mkdir :
	@mkdir -p $(DIR)

clean :
	@rm	-rf $(DIR)

reset : clean mkdir

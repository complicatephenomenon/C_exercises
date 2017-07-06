#include<stdio.h>
#include<assert.h>
/* conditionally compiled macro that compares its argument to zero */
#include<stdlib.h>
#include<errno.h>
/* macro reporting error conditions */
#include<string.h>
/* <stdio.h>  General utilities: memory management, program utilities, 
 * string conversions, random numbers */
#define MAX_DATA 512
#define MAX_ROWS 100

struct Address{
	int id;
	int set;
	char name[MAX_DATA];
	char email[MAX_DATA];
};

struct Database{
	struct Address rows[MAX_ROWS];
};
/* It seems that struct Database is same with struct Address*/
struct Connection{
	FILE *file;
	struct Database *db;
};

void die(const char *message)
{
	if(errno){
		perror(message);
	}
	else{
		printf("ERROR:%s\n", message);
	}

	exit(1);
}

void Address_print(struct Address *addr)
{
	printf("%d %s %s \n", addr->id, addr->name, addr->email);
}
/* addr is a pointer to a structure
 * We acess structure's member through pointer. 2 ways
 * 1. Referencing pointer to another address memory
 * 2. Using dynamic memory allocation */


/*if a is a pointer to a structure in which b is a member 
 * then you access b with (*a).b
 * This is such a common occurrence in C that a shorthand exists: a->b   */
void Database_load(struct Connection * conn)
{
	int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
	if(rc !=1)
		die("Failed to load database.");
}

struct Connection *Database_open(const char *filename, char mode)
{
	struct Connection *conn = malloc(sizeof(struct Connection));
	if(!conn)die("Memory error");
/*If conn is NULL, !conn will be non-NULL and the condition will be true.*/
	
	conn->db = malloc(sizeof(struct Database));
/* conn->db fetches the value of the member variable db 
 * * if conn->db is NULL(for pointers, NULL is falsy). !conn->db will be 
 * non-NULL and the condition will be true
 * * vice versa */
	if(!conn->db) die("Memory error");
/*The if (!conn->db) means the same as if (conn->db == 0) 
 * or if (conn->db == NULL)*/
/*  It works with any numeric or pointer type when you compare to 0 */
	if(mode == 'c')
	{
		conn -> file = fopen(filename, "w");
	}

	else{
		conn -> file = fopen(filename, "r+");
		if(conn->file){
			Database_load(conn);
		}
	}
	if(!conn -> file) die("Failed to open the file");

	return conn;
}

void Database_close(struct Connection *conn)
{
	if(conn){
		if(conn->file){
			fclose(conn->file);
		}
		if(conn->db) {
			free(conn->db);
		}
		free(conn);
	}
}

void Database_write(struct Connection *conn)
{
	rewind(conn->file);
	int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);

	if(rc != 1)
		die ("Failed to write database.");

	rc = fflush(conn-> file);
	if(rc ==-1)
		die("Cannot flush database.");
}

void Database_create(struct Connection *conn)
{
	int i = 0;

	for(i = 0; i < MAX_ROWS; i++){

		struct Address addr = {.id = i, .set = 0};

		conn ->db ->rows[i] = addr;
	}
}

void Database_set (struct Connection *conn, int id, const char *name, 
		const char * email)
{
	struct Address *addr = &conn->db->rows[id];
	if(addr->set)
		die("Already set, delete it first");

	addr->set = 1;

	char *res = strncpy(addr->name, name, MAX_DATA);

	if(!res)
		die("Name copy failed");

	res = strncpy(addr->email, email, MAX_DATA);
	if(!res) die("Email copy failed");
}
void Database_get(struct Connection *conn, int id)
{
	struct Address *addr = &conn->db->rows[id];
	if(addr->set){
		Address_print(addr);
	}
	else{
		die("ID is not set");
	}
}

void Database_delete(struct Connection *conn, int id)
{
	struct Address addr = {.id = id, .set = 0};
	conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn)
{
	int i = 0;
	struct Database *db = conn->db;

	for(i = 0; i < MAX_ROWS; i++){
		struct Address *cur = &db -> rows[i];

		if(cur -> set){
			Address_print(cur);
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3)
		die("USAGE: stack <dbfile> <action> [action params]");

	char *filename = argv[1];
	char action = argv[2][0];
	struct Connection *conn = Database_open(filename, action);
	int id = 0;

	if(argc > 3) id = atoi(argv[3]);
	if(id >= MAX_ROWS) die("There is not that many records.");

	switch(action){
		case 'c':
			Database_create(conn);
			Database_write(conn);
			break;
		case 'g':
			if(argc != 4) 
				die("Need an id to get ");

			Database_get(conn, id);
			break;
		case 's':
			if(argc != 6)
				die ("Need id , name, email to set");
			Database_set(conn, id , argv[4], argv[5]);
			Database_write(conn);
			break;
		case 'd':
			if(argc != 4) die("Need id to delete");

			Database_delete(conn, id);
			Database_write(conn);
			break;

		case 'l':
			Database_list(conn);
			break;
		default:
			die("Invalid action, only: c=create, g=get, s=set, d= del, l=list");
	}

	Database_close(conn);

	return 0;
}
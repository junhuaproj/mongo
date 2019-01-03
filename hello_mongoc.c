#include <mongoc/mongoc.h>
//静态pkg-config --libs --cflags libmongoc-static-1.0
//动态链接
//gcc -o hello_mongoc hello_mongoc.c $(pkg-config --libs --cflags libmongoc-1.0)
int main(int argc,char* argv[])
{
	const char* uri_string="mongodb://localhost:27017";
	mongoc_uri_t* uri;
	mongoc_client_t* client;
	mongoc_database_t* database;
	mongoc_collection_t* collection;
	bson_error_t error;
	bson_t* command,reply,*insert;
	char* str;
	bool retval;
	
	mongoc_init();
	if(argc>1)
	{
		uri_string=argv[1];
	}
	uri=mongoc_uri_new_with_error(uri_string,&error);
	if(!uri)
	{
		fprintf(stderr,"failed to parse uri:%s\nerror message:%s",
			uri_string,error.message);
		return EXIT_FAILURE;
	}
	client=mongoc_client_new_from_uri(uri);
	if(!client)
	{
		return EXIT_FAILURE;
	}
	mongoc_client_set_appname(client,"connect-example");
	database=mongoc_client_get_database(client,"db_name");
	collection=mongoc_client_get_collection(client,"db_name","coll_name");
	command=BCON_NEW("ping",BCON_INT32(1));
	retval=mongoc_client_command_simple(client,"admin",command,NULL,&reply,&error);
	if(!retval)
	{
		fprintf(stderr,"%s\n",error.message);
		return EXIT_FAILURE;
	}
	str=bson_as_json(&reply,NULL);
	printf("%s\n",str);
	insert=BCON_NEW("hello",BCON_UTF8("world"));
	if(!mongoc_collection_insert_one(collection,insert,NULL,NULL,&error)){
		fprintf(stderr,"%s\n",error.message);
	}
	bson_destroy(insert);
	bson_destroy(&reply);
	bson_destroy(command);
	bson_free(str);
	
	mongoc_collection_destroy(collection);
	mongoc_database_destroy(database);
	mongoc_uri_destroy(uri);
	mongoc_client_destroy(client);
	mongoc_cleanup();
	return EXIT_SUCCESS;
	
}


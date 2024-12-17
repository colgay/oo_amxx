#include <amxmodx>
#include <oo>

#define main plugin_init

public oo_init()
{
	// A base class for all animals
	oo_class("Animal")
	{
		new cl[] = "Animal";

		oo_var(cl, "age", 1); // A attribute that stores the age of the animal
		oo_var(cl, "name", 32); // A attribute that stores the name of the animal

		// A constructor that takes the name and the age of the animal
		oo_ctor(cl, "Ctor", @str(name), @int(age));

		// A destructor of the animal
		oo_dtor(cl, "Dtor");

		// A function that returns the sound of the animal
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// A function that returns the number of legs of the animal
		oo_mthd(cl, "GetLegs");

		// A function that prints the name and sound of the animal
		oo_mthd(cl, "Introduce");

		oo_smthd(cl, "Test"); // static method test
	}

	// A derived class for dogs that inherits from Animal
	oo_class("Dog", "Animal")
	{
		new cl[] = "Dog";

		// A constructor that calls the base class constructor with the name "Dog"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a dog
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a dog
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for cats that inherits from Animal
	oo_class("Cat", "Animal")
	{
		new cl[] = "Cat";

		// A constructor that calls the base class constructor with the name "Cat"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a cat
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a cat
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for birds that inherits from Animal
	oo_class("Bird", "Animal")
	{
		new cl[] = "Bird";

		// A constructor that calls the base class constructor with the name "Bird"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a bird
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		// An override function that returns the number of legs of a bird
		oo_mthd(cl, "GetLegs");
	}

	// A derived class for snakes that inherits from Animal
	oo_class("Snake", "Animal")
	{
		new cl[] = "Snake";

		// A constructor that calls the base class constructor with the name "Snake"
		oo_ctor(cl, "Ctor", @int(age));

		// An override function that returns the sound of a snake
		oo_mthd(cl, "MakeSound", @stref(msg), @int(len));

		oo_mthd(cl, "Test", OO_CELL, OO_BYREF, OO_STRING, OO_STRING_REF, OO_ARRAY[5]);

		// An override function that returns the number of legs of a snake
		oo_mthd(cl, "GetLegs");
	}
}

// A constructor of Animal
public Animal@Ctor(const name[], age)
{
	new this = oo_this(); // Get this object hash
	oo_set_str(this, "name", name); // Assign string value to the member variable of "name"
	oo_set(this, "age", age); // Assign integer value to the member variable of "age"
}

// A destructor of Animal
public Animal@Dtor()
{
	new name[32];
	oo_get_str(oo_this(), "name", name, charsmax(name)); // Get string value from the member variable of "name"
	server_print("%s has been euthanized.", name);
}

public Animal@MakeSound(msg[], len)
{
	// format the message to the msg[]
	formatex(msg, len, "I am an animal");
}

public Animal@GetLegs()
{
	return 0;
}

public Animal@Introduce()
{
	new this = oo_this();

	new name[32];
	oo_get_str(this, "name", name, charsmax(name));

	new age = oo_get(this, "age"); // Get integer value from the member variable of "age"

	new legs = oo_call(this, "GetLegs");

	// Call "MakeSound" function of this animal and retrieve the result to the msg[]
	new msg[64];
	oo_call(this, "MakeSound", msg, charsmax(msg));

	// Print the introduction
	server_print("Hello, my name is %s, I'm %d years old, I have %d legs, and I say %s", name, age, legs, msg);
}

public Animal@Test() { server_print("static method test"); }

public Dog@Ctor(age)
{
	// Call the parent constructor of this dog
	oo_super_ctor("Animal", "Dog", age);
}

public Dog@MakeSound(msg[], len)
{
	formatex(msg, len, "Woof woof");
}

public Dog@GetLegs()
{
	return 4;
}

public Cat@Ctor(age)
{
	oo_super_ctor("Animal", "Cat", age);
}

public Cat@MakeSound(msg[], len)
{
	formatex(msg, len, "Meow meow");
}

public Cat@GetLegs()
{
	return 4;
}

public Bird@Ctor(age)
{
	oo_super_ctor("Animal", "Bird", age);
}

public Bird@MakeSound(msg[], len)
{
	formatex(msg, len, "Tweet tweet");
}

public Bird@GetLegs()
{
	return 2;
}

public Snake@Ctor(age)
{
	oo_super_ctor("Animal", "Snake", age);
}

public Snake@MakeSound(msg[], len)
{
	formatex(msg, len, "Sss sss");
}

public Snake@GetLegs()
{
	return 0;
}

public Snake@Test(a, &b, const c[], d[], e[5])
{
	server_print("Snake@Test(%d, %d, %s, %s, {%d,%d,%d,%d,%d})", a, b, c ,d, e[0], e[1], e[2], e[3], e[4]);
}

public main()
{
	register_plugin("[OO] Animal", "0.1", "holla");

	new Animal:animals[5];
	animals[0] = oo_new("Dog", 7);
	animals[1] = oo_new("Cat", 6);
	animals[2] = oo_new("Bird", 4);
	animals[3] = oo_new("Snake", 3);
	animals[4] = oo_new("Animal", "Unknown", 0);

	for (new i = 0; i < 5; i++)
	{
		oo_call(animals[i], "Introduce"); // Call Introduce function for each animals
	}

	// Tests
	oo_call(0, "Animal@Test"); // Test calling the static method

	server_print("Object #%d %s a Snake", animals[3], oo_isa(animals[3], "Snake") ? "IS" : "IS NOT");
	server_print("Object #%d %s a Dog", animals[3], oo_isa(animals[3], "Dog") ? "IS" : "IS NOT");

	server_print("Class Dog %s a subclass of Animal", oo_subclass_of("Dog", "Animal") ? "IS" : "IS NOT");
	server_print("Class Animal %s a subclass of Cat", oo_subclass_of("Animal", "Cat") ? "IS" : "IS NOT");

	server_print("Class Bird %s", oo_class_exists("Bird") ? "EXISTS" : "DOES NOT EXIST");
	server_print("Class Fish %s", oo_class_exists("Fish") ? "EXISTS" : "DOES NOT EXIST");

	new class[32];
	oo_get_classname(animals[0], class, charsmax(class));
	server_print("Object #%d's classname is %s", animals[0], class);

	server_print("Object #%d %s", animals[0], oo_object_exists(animals[0]) ? "EXISTS" : "DOES NOT EXIST");

	new str[32] = "4";
	oo_call(animals[3], "Test", 1, 2, "3", str, {1, 2, 3, 4, 5});
	server_print("str is %s", str);

	for (new i = 0; i < 5; i++)
	{
		oo_delete(animals[i]); // Delete each animal objects
	}

	server_print("Object #%d %s", animals[0], oo_object_exists(animals[0]) ? "EXISTS" : "DOES NOT EXIST");
}
#if defined _oo_included
	#endinput
#endif

#define _oo_included

#pragma reqlib oo
#if !defined AMXMODX_NOAUTOLOAD
#pragma loadlib oo
#endif

#define OO_VERSION_NUM	130
stock const OO_VERSION_STR[] = "1.3.0";

#define @null 					any:0 		// null object
#define @this 					oo_this() 	// this object
#define @incre(%0[%1])			oo_set(%0, oo_get(%0, #%1) + 1) // increment
#define @decre(%0[%1])			oo_set(%0, oo_get(%0, #%1) - 1) // decrement

// OO Arg Types (Full name)
#define OO_CELL			(1)		// cell
#define OO_FLOAT		(1)		// float
#define OO_OBJECT		(1)		// object
#define OO_BYREF		(0) 	// cell or float by reference
#define OO_STRING		(-1) 	// const string[]
#define OO_STRING_REF 	(-2) 	// string[]
#define OO_VEC2			(2)		// 2d vector (not necessary)
#define OO_VEC3			(3)		// 3d vector (not necessary)
#define OO_ARRAY[%0]	(%0)	// array[size]
#define OO_STATIC		(0)		// for static method

// OO Arg Types (Short version with arg name)
#define @int(%0) 		OO_CELL
#define @bool(%0)		OO_CELL
#define @fl(%0)			OO_FLOAT
#define @ref(%0)		OO_BYREF
#define @str(%0)		OO_STRING
#define @stref(%0)		OO_STRING_REF
#define @vec(%0[%1])	OO_ARRAY[%1]
#define @arr(%0[%1])	OO_ARRAY[%1]
#define @obj(%0)		OO_OBJECT
#define @static 		OO_STATIC

/**
 * Get the value of the array-type variable from an object.
 * oo_get_arr(any:this, const name[], output_arr[])
 * Example:
 *   new array[3] = {3, 6, 9};
 *   oo_set(this, "items", array);
#define oo_get_arr(%0,%1,%2)		oo_get(%0, %1, 0, 0, %2, 0, 0)

/*
 * Set the value of the array-type variable to an object.
 * oo_set_arr(any:this, const name[], const array[])
 * Example:
 *   new array[32];  
 *   oo_set(this, "items", array);
 */
#define oo_set_arr(%0,%1,%2)		oo_set(%0, %1, 0, 0, %2, 0, 0)

// New macro
#define @range(%0..%1,%2[%3..%4]) 	%0, %1, %2, %3, %4
#define @array(%0,%1)				0, 0, %0, 0, %1
// new item[3];
// oo_set(this, "items", @range(0..3, items[0..3]))
// oo_set(this, "items", @array(items, 3))

enum
{
	OO_CONTINUE = 0,
	OO_SUPERCEDE,
	OO_BREAK
};

/**
 * Called just after server activation.
 *
 * @note Good place to initialize most of the class
 *
 * @noreturn
 */
forward oo_init();

/**
 * Declare a class.
 *
 * @note Declare a class:
 *			oo_class("Animal");
 *		 Declare a subclass: (inherited from "Animal")
 *		    oo_class("Monkey", "Animal");
 *
 * @note Multiple inheritance: (inherited from "A" and "B")
 *			oo_class("C", "A", "B");
 *
 * @param class   	Class name
 * @param ... 		Super classes
 *
 * @noreturn
 */
native oo_class(const class[], any:...);

/**
 * Declare a constructor of a class.
 *
 * @note Declare with no arg:
 *			oo_ctor("Animal", "Ctor");
 *		 Declare with args:
 *		    oo_ctor("Animal", "Ctor", @int(age), @str(name));
 *
 * @note AMXX function callback:
 * 			public Animal@Ctor(age, const name[]) {}
 *
 * @note Creating an object:
 * 			oo_new("Animal", 5, "Lucy");
 *
 * @param class   	Class name
 * @param name   	Constructor name
 * @param ... 		Args of this constructor
 *
 * @noreturn
 */
native oo_ctor(const class[], const name[], any:...);

/**
 * Declare a destructor of a class.
 *
 * @note Declaration:
 *			oo_dtor("Animal", "Dtor")
 *
 * @note AMXX function callback:
 * 			public Animal@Dtor() {}
 *
 * @param class   	Class name
 * @param name   	Destructor name
 *
 * @noreturn
 */
native oo_dtor(const class[], const name[]);

/**
 * Declare a method of a class.
 *
 * @note Declare with no arg:
 *			oo_mthd("Animal", "MakeSound")
 *		 Declare with args:
 *		    oo_mthd("Animal", "MakeSound", @fl(volume));
 *
 * @note AMXX function callback:
 * 			public Animal@MakeSound(Float:volume) {}
 *
 * @note Calling a method:
 * 			oo_call(object, "MakeSound", 0.75);
 *
 * @param class   	Class name
 * @param name   	Method name
 * @param ... 		Args of this method
 *
 * @noreturn
 */
native oo_mthd(const class[], const name[], any:...);

/**
 * Declare a static method of a class.
 *
 * @note Declare with no arg:
 *			oo_smthd("Animal", "Static")
 *		 Declare with args:
 *		    oo_smthd("Animal", "Static", @byref(val), @stref(out), @int(len));
 *
 * @note AMXX function callback:
 * 			public Animal@MakeSound(&val, out[], len) {}
 *
 * @note Calling a static method:
 *			new val, out[32]
 * 			oo_call(@static, "Animal@Static", val, out, charsmax(out));
 *
 * @param class   	Class name
 * @param name   	Method name
 * @param ... 		Args of this method
 *
 * @noreturn
 */
native oo_smthd(const class[], const name[], any:...);

/**
 * Declare a variable of a class.
 *
 * @note Examples:
 *			oo_var("Animal", "cell_var", 1) // for a cell or float (1 size)
 * 			oo_var("Animal", "array_var", 32) // for an array or a string (32 size)
 *
 * @param class   	Class name
 * @param name   	Variable name
 * @param size 		Size of this variable
 *
 * @noreturn
 */
native oo_var(const class[], const name[], size=1);

/**
 * Check if an object belongs to a specific class.
 *
 * @param this   		Object hash
 * @param _class   		Class name
 * @param superclass 	When superclass=true, it also check for the parent classes.
 *
 * @return 				true if object is belong to this class, false otherwise
 */
native bool:oo_isa(any:this, const _class[], bool:superclass=true);

/**
 * Check if a class is a subclass of another class.
 *
 * @param sub   		Class name to check
 * @param _super   		Another class name
 *
 * @return 				true if sub is a subclass of _super, false otherwise
 */
native bool:oo_subclass_of(const sub[], const super[]);

/**
 * Check if a class exists.
 *
 * @param class   		Class name to check
 *
 * @return 				true if this class exists, false otherwise
 */
native bool:oo_class_exists(const class[]);

/**
 * Check if an object exists.
 *
 * @param this   		Object hash
 *
 * @return 				true if this object exists, false otherwise
 */
native bool:oo_object_exists(any:this);

/**
 * Get the class name from an object.
 *
 * @param this   		Object hash
 * @param output   		Buffer to copy string to
 * @param len			Maximum buffer size
 *
 * @noreturn
 */
native oo_get_classname(any:this, output[], len);

/**
 * Get the class name from an object.
 *
 * @note For exmaple, if your constructor was declared like this:
 * 			oo_ctor("Dog", "Ctor", @str{name}, @int{weight}, @fl{height});
 * 		 Creating an object like this:
 * 			oo_new("Dog", "Luna", 110, 66.0);
 *
 * @param class   		Class name
 * @param ...			Args of class constructor
 *
 * @noreturn
 */
native any:oo_new(const class[], any:...);

/**
 * Delete an object.
 *
 * @note This also call the destructor from the object.
 *
 * @param this   		Object hash
 *
 * @noreturn
 */
native oo_delete(any:this);

/**
 * Call a method from an object.
 *
 * @note For exmaple, if your method was declared like this:
 * 			oo_mthd("Dog", "Eat", @str{food_name}, @int{amount});
 * 		 Calling a method like this:
 * 			oo_call(this, "Eat", "apple", 2);
 *
 * @note Calling a parent method: ("Animal" is the parent class)
 * 			oo_call(this, "Animal@Eat", "orange", 1);
 *
 * @note Calling a static method:
 * 			oo_call(@static, "Animal@StaticMethod");
 *
 * @param this   		Object hash
 * @param name   		Method name (You can specify the class name like "Animal@Method")
 * @param ...			Args of this method
 *
 * @return 				The returns of this call
 */
native oo_call(any:this, const name[], any: ...);

/* Get the value of a variable from an object.
 *   oo_get(any:this, const name[], begin, end, output[], begin, end)
 * 
 * For one cell variable:
 *   new hp = oo_get(this, "health");
 *   new Float:speed = Float:oo_get(this, "speed");
 *
 * For array or string:
 *   new arr[3], str[32];
 *   oo_get(this, "test_array", 0, 3, arr, 0, 3);
 *   oo_get(this, "test_string", 0, 32, str, 0, 32);
 *
 * For parent variable:
 *   new hp = oo_get(this, "Base@health");
 * 
 * Get the values within a specific range of indexes from an array.
 *   new arr[5] = {1, 2, 3, 4, 5};
 *   oo_set(this, "test_array", 4, 9, arr, 1, 5);
 *
 * The above code retrieves values from the test_array[] (index 4 to 8) and outputs them to the arr[] (index 1 to 4)
 * For the simple version, please use the marco `oo_get_arr` and `oo_get_str`.
 */

/**
 * Get the value of a variable from an object.
 *
 * @note Getting one cell:
 * 			new hp = oo_get(this, "health");
 *			new Float:speed = Float:oo_get(this, "speed");
 *
 * @note Getting an array:
 * 			new Float:pos[3], arr[10];
 *			oo_get(this, "origin", @array(pos, 3));
 *		 From a range: (0 ~ 5)
 *			oo_get(this, "array", @range(0..5, arr[0..5]));
 *
 * @note Getting a parent variable:
 * 			new age = oo_get(this, "Animal@age");
 *
 * @param this   		Object hash
 * @param name   		Variable name (You can specify the class name like "Animal@var")
 * @param ...			See above
 *
 * @return 				Value of this variable (cell)
 */
native oo_get(any:this, const name[], any: ...);

/**
 * Set the value for a variable from an object.
 *
 * @note Setting one cell:
 * 			oo_set(this, "health", 150);
 *			oo_set(this, "speed", 270.0);
 *
 * @note Setting an array:
 * 			new Float:pos[3], arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
 *			pev(id, pev_origin, pos);
 *			oo_set(this, "origin", @array(pos, 3));
 *		 From a range: (0 ~ 5)
 *			oo_set(this, "array", @range(0..5, arr[0..5]));
 *
 * @note Setting a parent variable:
 * 			oo_set(this, "Animal@age", 10);
 *
 * @param this   		Object hash
 * @param name   		Variable name (You can specify the class name like "Animal@var")
 * @param ...			See above
 *
 * @noreturn 
 */
native oo_set(any:this, const name[], any: ...);

/**
 * Get the string for a variable from an object.
 *
 * @note Examples:
 *			// General
 *			new string[32];
 * 			oo_get(this, "name", string, charsmax(string));
 *
 *			// If the "desc" is "This apple is red." and I only want the "red"
 * 			oo_get(this, "desc", "This apple is red.", 3, 14);
 *
 *
 * @param this   		Object hash
 * @param name   		Variable name (You can specify the class name like "Animal@var")
 * @param string 		String
 * @param length		Start position
 * @param start			Length (0 = Auto detect)
 *
 * @return 				Number of cells written to variable
 */
native oo_get_str(any:this, const name[], string[], length, start=0);

/**
 * Set the string for a variable from an object.
 *
 * @note Examples:
 *			// General
 * 			oo_set(this, "name", "Macy");
 *
 * 			// Append the string
 *			new len = 0;
 * 			len += oo_set(this, "buffer", "first line\n");
 * 			len += oo_set(this, "buffer", "second line\n", len);
 *
 *			// Only "apple" is set to the variable
 * 			len += oo_set(this, "buffer", "This apple is red.", 6, 5);
 *
 * @param this   		Object hash
 * @param name   		Variable name (You can specify the class name like "Animal@var")
 * @param string 		String
 * @param start			Start position
 * @param length		Length (0 = Auto detect)
 *
 * @return 				Number of cells written to variable
 */
native oo_set_str(any:this, const name[], const string[], start=0, length=0);

/**
 * Get (THIS) object of the current callstack
 * @note You can only use this inside an OO callback.
 *
 * @return 				Object hash
 */
native any:oo_this();

/**
 * Call a parnet constructor.
 *
 * @note You can only use this inside an OO callback.
 *
 * @param super			Parent class name
 * @param ...			Args of this constructor
 *
 * @noreturn
 */
native oo_super_ctor(const super[], any: ...);

/**
 * Register a hook for a method.
 *
 * @param class			Class name
 * @param name			Method name
 * @param callback 		An AMXX callback
 * @param post 			1 = Hook as post, 0 = pre
 *
 * @noreturn
 */
native oo_hook_mthd(const class[], const name[], const callback[], post=0);

/**
 * Register a hook for a constructor.
 *
 * @param class			Class name
 * @param name			Constructor name
 * @param callback 		An AMXX callback
 * @param post 			1 = Hook as post, 0 = pre
 *
 * @noreturn
 */
native oo_hook_ctor(const class[], const name[], const callback[], post=0);

/**
 * Register a hook for a destructor.
 *
 * @param class			Class name
 * @param callback 		An AMXX callback
 * @param post 			1 = Hook as post, 0 = pre
 *
 * @noreturn
 */
native oo_hook_dtor(const class[], const callback[], post=0);

/**
 * Set the return result for the current method. 
 * Only work in the pre hook and you must return either OO_SUPERCEDE or OO_BREAK in the hook.
 *
 * @note You can only use this inside an OO callback.
 *
 * @param value			The value you want to set
 *
 * @noreturn
 */
native oo_hook_set_return(any:value);

/**
 * Get the return result for current method. (Work only in post hook)
 *
 * @note You can only use this inside an OO callback.
 *
 * @return value		The result
 */
native any:oo_hook_get_return();

/**
 * Set a parameter value inside a hook. 
 * Only work for cell and string type, array and reference type can directly modify.
 *
 * @note Examples:
 *			...
 *			oo_mthd("Animal", "Method", @int(cell), @str(str))
 *			oo_hook_mthd("Animal", "Method", "OnAnimalMethod");
 *			...
 *
 *			public OnAnimalMethod(cell, const str[])
 *			{
 *				oo_hook_set_param(1, OO_CELL, 1234);
 *				oo_hook_set_param(2, OO_STRING, "I'm not an animal actually.");
 *			}
 *
 * @note You can only use this inside an OO callback.
 *
 * @noreturn;
 */
native oo_hook_set_param(param, type, any: ...);

/**
 * Get the size of a variable from an object.
 *
 * @param this		Object hash
 * @param name		Variable name
 *
 * @return 			The size of this variable
 */
native oo_get_var_size(any:this, const name[]);

/**
 * Get the string length of a variable from an object.
 *
 * @param this		Object hash
 * @param name		Variable name
 *
 * @return 			The size of this variable
 */
native oo_get_str_len(any:this, const name[]);

/**
 * Get the plugin id of current callstack. (You can only use this inside an OO callback)
 *
 * @return 			Plugin ID
 */
native oo_get_plugin();

/**
 * Append the string
 */
stock oo_add_str(any:this, const name[], const string[], pos=-1)
{
	return oo_set_str(this, name, string, (pos == -1) ? oo_get_str_len(this, name) : pos);
}
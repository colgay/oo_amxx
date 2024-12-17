// 
//		OO (Object-Oriention) support as a module enabled for AMX Mod X plugins.
//		Copyright (C) 2022  Hon Fai and holla
// 
//		This program is free software: you can redistribute itand /or modify 
//		it under the terms of the GNU General Public License as published by 
//		the Free Software Foundation, either version 3 of the License, or 
//		(at your option) any later version.
// 
//		This program is distributed in the hope that it will be useful, 
//		but WITHOUT ANY WARRANTY; without even the implied warranty of 
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//		GNU General Public License for more details.
// 
//		You should have received a copy of the GNU General Public License 
//		along with this program. If not, see <https://www.gnu.org/licenses/>.
// 

#if defined _oo_included
	#endinput
#endif

#define _oo_included

#pragma reqlib oo
#if !defined AMXMODX_NOAUTOLOAD
#pragma loadlib oo
#endif

#define OO_VERSION		1.1.0
#define OO_VERSION_NUM	110
stock const OO_VERSION_STR[] = "1.1.0";

#define @null 					any:0 // null object
#define @this 					oo_this() // this object
#define @increment(%0[%1]) 		oo_set(%0, oo_get(%0, %1) + 1)
#define @decrement(%0[%1]) 		oo_set(%0, oo_get(%0, %1) - 1)

#define OO_CELL			(1)		// cell
#define OO_FLOAT		(1)		// float
#define OO_OBJECT		(1)		// object
#define OO_BYREF		(0) 	// cell or float by reference
#define OO_STRING		(-1) 	// const string[]
#define OO_STRING_REF 	(-2) 	// string[]
#define OO_VEC2			(2)		// 2d vector (not necessary)
#define OO_VEC3			(3)		// 3d vector (not necessary)
#define OO_ARRAY[%0]	(%0)	// array[size]

#define @int(%0) 		OO_CELL
#define @bool(%0)		OO_CELL
#define @fl(%0)			OO_FLOAT
#define @ref(%0)		OO_BYREF
#define @str(%0)		OO_STRING
#define @stref(%0)		OO_STRING_REF
#define @vec(%0[%1])	OO_ARRAY[%1]
#define @arr(%0[%1])	OO_ARRAY[%1]
#define @obj(%0)		OO_OBJECT

// Get the value of the array-type variable from an object.
// oo_get_arr(any:this, const name[], output_arr[])
// Examples:
//   new array[3] = {3, 6, 9};
//   oo_set(this, "items", array);
#define oo_get_arr(%0,%1,%2)		oo_get(%0, %1, 0, 0, %2, 0, 0)

// Set the value of the array-type variable from an object.
// oo_set_arr(any:this, const name[], const array[])
// Examples:
//   new array[32];  
//   oo_set(this, "items", array);
#define oo_set_arr(%0,%1,%2)		oo_set(%0, %1, 0, 0, %2, 0, 0)

// new macro
#define @range(%0..%1,%2[%3..%4]) 	%0, %1, %2, %3, %4
#define @array(%0,%1)				0, 0, %0, 0, %1
#define @string(%0,%1)				@array(%0, %1)
#define @len(%0)					sizeof %0

// oo_set(this, "items", @range(0..2, items[0..2]))
// oo_set(this, "items", @array(items, 0))
// oo_set(this, "items", @string("fuck", 0))
// oo_get(this, "class", @string(name, @len(name)))

enum
{
	OO_CONTINUE = 0,
	OO_SUPERCEDE,
	OO_BREAK
};

forward oo_init();

/* Declare a class:
 *   oo_class("Animal");
 *	
 * Declare a subclass (inherited from "Animal"):
 *   oo_class("Monkey", "Animal");
 *	
 * Multiple inheritance
 *   oo_class("Base", "A", "B");
 */
native oo_class(const class[], any:...);

/* Declare a constructor of a class:
 *
 * No parameters:
 *   oo_ctor("Animal", "Ctor");
 *
 * With parameters:
 *   oo_ctor("Animal", "Ctor", @str{name}, @int{weight}, @fl{height});
 *
 * Afterwards, you will need to declare a public function as follows:
 *   public Animal@Ctor(const name[], weight, Float:height) {}
 */
native oo_ctor(const class[], const name[], any:...);

/* Declare a destructor of a class
 *   oo_dtor("Animal", "Dtor")
 *
 * Afterwards, you will need to declare a public function as follows:
 *   public Animal@Dtor() {}
 */
native oo_dtor(const class[], const name[]);

/* Declare a method of a class:
 *
 * No paramters:
 *   oo_ctor("Animal", "Eat");
 *
 * With parameters:
 *   oo_ctor("Animal", "Eat", @str{food_name}, @int{amount});
 *
 * Afterwards, you will need to declare a public function as follows:
 *   public Animal@Eat(const food_name[], amount) {}
 */
native oo_mthd(const class[], const name[], any:...);

/* Declare a static method of a class (unnecessary?) */
native oo_smthd(const class[], const name[], any:...);

/* Declare a variable of a class
 *
 * For one cell (like `new int_var` `new Float:fl_var`) :
 *   oo_var("Animal", "int_var", 1);
 *   oo_var("Animal", "fl_var", 1);
 *
 * For array or string (like `new array[10]` `new string[32]`) :
 *   oo_var("Animal", "array", 10);
 *   oo_var("Animal", "string", 32);
 */
native oo_var(const class[], const name[], size=1);

/* Check if an object belongs to a specific class.
 *   oo_isa(object, "Animal")
 * With superclass=true, it also checks for the parent class.
 */
native bool:oo_isa(any:this, const _class[], bool:superclass=true);

/* Check if a class is a subclass of another class.
 *   oo_subclass_of("Monkey", "Animal")
 * The above code checks if "Monkey" is a subclass of "Animal".
 */
native bool:oo_subclass_of(const sub[], const _super[]);

/* Check if a class exists */
native bool:oo_class_exists(const class[]);

/* Check if an object exists */
native bool:oo_object_exists(any:this);

/* Check if a method exists in a object */
//native bool:oo_method_exists(any:this, const mthd_name[]);

/* Check if a variable exists in a object */
//native bool:oo_var_exists(any:this, const var_name[]);

/* Get class name from a object */
native oo_get_classname(any:this, output[], len);

/* Create an object
 *
 * For example, if the constructor was declared like this:
 *   oo_ctor("Dog", "Ctor", @str{name}, @int{weight}, @fl{height});
 *
 * So, create an object like this:
 *   new object = oo_new("Dog", "Luna", 110, 66.0);
 */
native any:oo_new(const class[], any: ...);

/* Delete an object */
native oo_delete(any:this);

/* Call a method from an object
 *
 * For example, if the method was declared like this:
 *   oo_mthd("Dog", "Eat", @str{food_name}, @int{amount});
 *
 * So, call a method like this:
 *   oo_call(this, "Eat", "apple", 2);
 *
 * For calling a parent method: ("Animal" is the parent class)
 *   oo_call(this, "Animal@Eat", "orange", 1);
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
native oo_get(any:this, const name[], any: ...);

/* Set the value of a variable from an object.
 *   oo_set(any:this, const name[], begin, end, const value[], begin, end)
 * 
 * For one cell:
 *   oo_set(this, "health", 100);
 *   oo_set(this, "speed", 250.0);
 *
 * For array or string:
 *   new const arr[3] = {3, 6, 9};
 *   oo_set(this, "test_array", 0, 3, arr, 0, 3);
 *   oo_set(this, "test_string", 0, 32, "I Love AMXX", 0, 32);
 *
 * For parent variable:
 *   oo_set(this, "Base@health", 50);
 * 
 * Set the values within a specific range of indexes to an array.
 *   new arr[5] = {1, 2, 3, 4, 5};
 *   oo_set(this, "test_array", 4, 9, arr, 1, 5);
 *
 * The above code set values from the arr[] (index 1 to 4) to the test_array[] (index 4 to 8);
 * result: test_array[] = {0, 0, 0, 0, 2, 3, 4, 5, ...}
 * For the simple version, please use the marco `oo_set_arr` and `oo_set_str`.
 */
native oo_set(any:this, const name[], any: ...);

// Get the value of the string-type variable from an object.
// oo_get_str(object, const name[], output_str[], len)
// Examples:
//   new name[32];
//   oo_get_str(this, "name", name, sizeof name);
native oo_get_str(any:this, const name[], string[], length, start=0);

// Set the value of the string-type variable from an object.
// oo_set_str(any:this, const name[], const string[])
// Examples:
//   oo_set_str(this, "name", "Peter");
native oo_set_str(any:this, const name[], const string[], start=0, length=0);

/* Get this object */
native any:oo_this();

/* Call parent constructor
 *   oo_super_ctor("Animal")
 */
native oo_super_ctor(const _super[], any: ...);

native oo_hook_mthd(const class[], const mthd_name[], const public_name[], post=0);

native oo_hook_ctor(const class[], const ctor_name[], const public_name[], post=0);

native oo_hook_dtor(const class[], const public_name[], post=0);

native oo_hook_set_return(any:value);

native any:oo_hook_get_return();

// example
// oo_hook_set_param(1, OO_CELL, 1337);
// oo_hook_set_param(2, OO_STRING, "good game");
native oo_hook_set_param(param, type, any: ...);

native oo_get_var_size(any:this, const name[]);

native oo_get_str_len(any:this, const name[]);

native oo_get_plugin();

stock oo_add_str(any:this, const name[], const string[], pos=-1)
{
	if (pos == -1)
		pos = oo_get_str_len(this, name);
	
	return oo_set_str(this, name, string, pos);
}
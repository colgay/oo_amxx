#include <amxmodx>
#include <oo>

//#define DEBUG

#if defined DEBUG
	#define __debug(%0) log_amx(%0)
#else
	#define __debug(%0)
#endif

enum GarbageType
{
	GBType_Array,
	GBType_Trie,
	GBType_Object,
};

enum _:OOGarbage_e
{
	GarbageType:OOGB_Type,
	OOGB_Name[32],
	OOGB_ArrSize,
};

new Trie:g_ooGarbages;

public plugin_init()
{
	register_plugin("OO Variable Extension", OO_VERSION_STR, "holla");
}

public plugin_natives()
{
	register_native("oo_var_array", "native_var_array");
	register_native("oo_var_trie", "native_var_trie");
	register_native("oo_var_object", "native_var_object");

	g_ooGarbages = TrieCreate();
}

public native_var_array()
{
	static class[32], name[32];
	get_string(1, class, charsmax(class));
	get_string(2, name, charsmax(name));

	new size = get_param(3);
	oo_var(class, name, size);

	new arr_size = get_param(4);
	RegisterOOGarbage(GBType_Array, class, name, arr_size);
}

public native_var_trie()
{
	static class[32], name[32];
	get_string(1, class, charsmax(class));
	get_string(2, name, charsmax(name));

	new size = get_param(3);
	oo_var(class, name, size);

	RegisterOOGarbage(GBType_Trie, class, name);
}

public native_var_object()
{
	static class[32], name[32];
	get_string(1, class, charsmax(class));
	get_string(2, name, charsmax(name));

	new size = get_param(3);
	oo_var(class, name, size);

	RegisterOOGarbage(GBType_Object, class, name);
}

public oo_object_created(const class[], num_args, any:this)
{
	__debug("--- oo_object_created(%s) start ---", class)

	static Array:garbages;
	if (TrieGetCell(g_ooGarbages, class, garbages))
	{
		static data[OOGarbage_e], varsize;
		for (new i = ArraySize(garbages)-1, j; i >= 0; i--)
		{
			ArrayGetArray(garbages, i, data);

			switch (data[OOGB_Type])
			{
				case GBType_Array:
				{
					varsize = oo_get_var_size(this, data[OOGB_Name]);
					for (j = 0; j < varsize; j++)
					{
						static Array:arr;
						oo_set(this, data[OOGB_Name], @range(j..j+1, ArrayCreate(data[OOGB_ArrSize])[0..1]));
						oo_get(this, data[OOGB_Name], @range(j..j+1, arr[0..1]))
						__debug("%s@%s ARRAY (%d) created", class, data[OOGB_Name], arr)
					}
				}
				case GBType_Trie:
				{
					varsize = oo_get_var_size(this, data[OOGB_Name]);
					for (j = 0; j < varsize; j++)
					{
						static Trie:tri;
						oo_set(this, data[OOGB_Name], @range(j..j+1, TrieCreate()[0..1]));
						oo_get(this, data[OOGB_Name], @range(j..j+1, tri[0..1]))
						__debug("%s@%s TRIE (%d) created", class, data[OOGB_Name], tri)
					}
				}
			}
		}
	}

	__debug("--- oo_object_created(%s) end ---^n", class)
}

public oo_object_deleted(const class[], any:this)
{
	__debug("--- oo_object_deleted(%s) start ---", class)

	static Array:garbages;
	if (TrieGetCell(g_ooGarbages, class, garbages))
	{
		static data[OOGarbage_e], varsize;
		for (new i = ArraySize(garbages)-1, j; i >= 0; i--)
		{
			ArrayGetArray(garbages, i, data);

			switch (data[OOGB_Type])
			{
				case GBType_Array:
				{
					varsize = oo_get_var_size(this, data[OOGB_Name]);
					for (j = 0; j < varsize; j++)
					{
						static Array:arr;
						oo_get(this, data[OOGB_Name], @range(j..j+1, arr[0..1]))
						if (arr != Invalid_Array)
						{
							__debug("%s@%s ARRAY (%d) deleted", class, data[OOGB_Name], arr)
							ArrayDestroy(arr);
						}
					}
				}
				case GBType_Trie:
				{
					varsize = oo_get_var_size(this, data[OOGB_Name]);
					for (j = 0; j < varsize; j++)
					{
						static Trie:tri;
						oo_get(this, data[OOGB_Name], @range(j..j+1, tri[0..1]))
						if (tri != Invalid_Trie)
						{
							__debug("%s@%s TRIE (%d) deleted", class, data[OOGB_Name], tri)
							TrieDestroy(tri);
						}
					}
				}
				case GBType_Object:
				{
					varsize = oo_get_var_size(this, data[OOGB_Name]);
					for (j = 0; j < varsize; j++)
					{
						static Object:obj;
						oo_get(this, data[OOGB_Name], @range(j..j+1, obj[0..1]))
						if (oo_object_exists(obj))
						{
							__debug("%s@%s OBJECT (%d) deleted", class, data[OOGB_Name], obj)
							oo_delete(obj);
						}
					}
				}
			}
		}
	}

	__debug("--- oo_object_deleted(%s) end ---^n", class)
}

RegisterOOGarbage(GarbageType:type, const class[], const name[], size=0)
{
	new Array:garbages;
	if (!TrieGetCell(g_ooGarbages, class, garbages))
	{
		garbages = ArrayCreate(OOGarbage_e);
		TrieSetCell(g_ooGarbages, class, garbages);
	}

	static data[OOGarbage_e];
	data[OOGB_Type] = type;
	data[OOGB_ArrSize] = size;
	copy(data[OOGB_Name], charsmax(data[OOGB_Name]), name);

	ArrayPushArray(garbages, data);
}
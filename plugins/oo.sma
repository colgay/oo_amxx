#include <amxmodx>
#include <oo>

public plugin_init()
{
	register_plugin("OO", OO_VERSION_STR, "holla");

	register_srvcmd("oo", "CmdOO");
}

public CmdOO()
{
	static arg1[32];
	read_argv(1, arg1, charsmax(arg1));

	if (equal(arg1, "version"))
	{
		static version[32];
		oo_version(version, charsmax(version));
		server_print("[OO] Version: %s", version);
		return PLUGIN_HANDLED;
	}

	if (equal(arg1, "class"))
	{
		static arg2[32];
		read_argv(2, arg2, charsmax(arg2))

		if (!oo_class_exists(arg2))
		{
			server_print("[OO] Class (%s) not exist", arg2);
			return PLUGIN_HANDLED;
		}

		oo_print(arg2);
		return PLUGIN_HANDLED;
	}

	if (equal(arg1, "classes"))
	{
		oo_print();
		return PLUGIN_HANDLED;
	}

	server_print("[OO] Unknown command");
	return PLUGIN_HANDLED;
}
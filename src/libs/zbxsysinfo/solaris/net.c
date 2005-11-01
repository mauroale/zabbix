/*
 * ** ZABBIX
 * ** Copyright (C) 2000-2005 SIA Zabbix
 * **
 * ** This program is free software; you can redistribute it and/or modify
 * ** it under the terms of the GNU General Public License as published by
 * ** the Free Software Foundation; either version 2 of the License, or
 * ** (at your option) any later version.
 * **
 * ** This program is distributed in the hope that it will be useful,
 * ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * ** GNU General Public License for more details.
 * **
 * ** You should have received a copy of the GNU General Public License
 * ** along with this program; if not, write to the Free Software
 * ** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * **/

#include "config.h"

#include "common.h"
#include "sysinfo.h"

static int get_kstat_named_field(
    const char *name, 
    const char *field,
    kstat_named_t *returned_data
    )
{
    int ret = SYSINFO_RET_FAIL;
    
    kstat_ctl_t	  *kc;
    kstat_t       *kp;
    kstat_named_t *kn;
		  
    kc = kstat_open();
    if (kc)
    {
	kp = kstat_lookup(kc, NULL, -1, (char*) name);
        if ((kp) && (kstat_read(kc, kp, 0) != -1))
	{
	    kn = (kstat_named_t*) kstat_data_lookup(kp, (char*) field);
	    if(kn)
	    {
            	*returned_data = *kn;
            	ret = SYSINFO_RET_OK;
	    }
        }
	kstat_close(kc);
    }
    return ret;
}

int	NET_IF_IN(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{

#define NET_FNCLIST struct net_fnclist_s
NET_FNCLIST
{
	char *mode;
	int (*function)();
};

	NET_FNCLIST fl[] = 
	{
		{"bytes",   NET_IF_IN_BYTES},
		{"packets", NET_IF_IN_PACKETS},
		{"errors",  NET_IF_IN_ERRORS},
		{0,	    0}
	};

	char interface[MAX_STRING_LEN];
	char mode[MAX_STRING_LEN];
	int i;
	
        assert(result);

        clean_result(result);
	
        if(num_param(param) > 2)
        {
                return SYSINFO_RET_FAIL;
        }

        if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
        {
                return SYSINFO_RET_FAIL;
        }
	
	if(get_param(param, 2, mode, MAX_STRING_LEN) != 0)
        {
                mode[0] = '\0';
        }
        if(mode[0] == '\0')
	{
		/* default parameter */
		sprintf(mode, "bytes");
	}
	
	for(i=0; fl[i].mode!=0; i++)
	{
		if(strncmp(mode, fl[i].mode, MAX_STRING_LEN)==0)
		{
			return (fl[i].function)(cmd, interface, flags, result);
		}
	}
	
	return SYSINFO_RET_FAIL;
}
int	NET_IF_OUT(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{

#define NET_FNCLIST struct net_fnclist_s
NET_FNCLIST
{
	char *mode;
	int (*function)();
};

	NET_FNCLIST fl[] = 
	{
		{"bytes",   NET_IF_OUT_BYTES},
		{"packets", NET_IF_OUT_PACKETS},
		{"errors",  NET_IF_OUT_ERRORS},
		{0,	    0}
	};

	char interface[MAX_STRING_LEN];
	char mode[MAX_STRING_LEN];
	int i;
	
        assert(result);

        clean_result(result);
	
        if(num_param(param) > 2)
        {
                return SYSINFO_RET_FAIL;
        }

        if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
        {
                return SYSINFO_RET_FAIL;
        }
	
	if(get_param(param, 2, mode, MAX_STRING_LEN) != 0)
        {
                mode[0] = '\0';
        }
        if(mode[0] == '\0')
	{
		/* default parameter */
		sprintf(mode, "bytes");
	}
	
	for(i=0; fl[i].mode!=0; i++)
	{
		if(strncmp(mode, fl[i].mode, MAX_STRING_LEN)==0)
		{
			return (fl[i].function)(cmd, interface, flags, result);
		}
	}
	
	return SYSINFO_RET_FAIL;
}

int	NET_IF_IN_BYTES(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "rbytes64", &kn);
    if (ret == SYSINFO_RET_OK)
    {
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui64;
    }
    else
    {
	ret = get_kstat_named_field(interface, "rbytes", &kn);
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui32;
    }
    
    return ret;
}

int	NET_IF_IN_PACKETS(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "ipackets64", &kn);
    if (ret == SYSINFO_RET_OK)
    {
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui64;
    }
    else
    {
	ret = get_kstat_named_field(interface, "ipackets", &kn);
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui32;
    }
    
    return ret;
}

int	NET_IF_IN_ERRORS(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "ierrors", &kn);

    result->type |= AR_DOUBLE;
    result->dbl = (double)kn.value.ui32;
    
    return ret;
}

int	NET_IF_OUT_BYTES(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "obytes64", &kn);
    if (ret == SYSINFO_RET_OK)
    {
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui64;
    }
    else
    {
	ret = get_kstat_named_field(interface, "obytes", &kn);
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui32;
    }
    
    return ret;
}

int	NET_IF_OUT_PACKETS(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "opackets64", &kn);
    if (ret == SYSINFO_RET_OK)
    {
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui64;
    }
    else
    {
	ret = get_kstat_named_field(interface, "opackets", &kn);
	result->type |= AR_DOUBLE;
        result->dbl = (double)kn.value.ui32;
    }
    
    return ret;
}

int	NET_IF_OUT_ERRORS(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);

    clean_result(result);
	
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "oerrors", &kn);

    result->type |= AR_DOUBLE;
    result->dbl = (double)kn.value.ui32;
    
    return ret;
}

int	NET_IF_COLLISIONS(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    kstat_named_t kn;
    char    interface[MAX_STRING_LEN];
    int	    ret;

    assert(result);
    
    clean_result(result);
    
    if(num_param(param) > 1)
    {
	return SYSINFO_RET_FAIL;
    }

    if(get_param(param, 1, interface, MAX_STRING_LEN) != 0)
    {
	return SYSINFO_RET_FAIL;
    }
    
    ret = get_kstat_named_field(interface, "collisions", &kn);

    if(ret == SYSINFO_RET_OK)
    {
	result->type |= AR_DOUBLE;
	result->dbl = (double)kn.value.ui32;
    }
    
    return ret;
}

int	NET_TCP_LISTEN(const char *cmd, const char *param, unsigned flags, AGENT_RESULT *result)
{
    char command[MAX_STRING_LEN];

    assert(result);
    
    clean_result(result);
    
    memset(command, '\0', sizeof(command));

    snprintf(command, sizeof(command)-1, "netstat -an | grep '*.%s' | wc -l", param);
   
    return EXECUTE(NULL, command, flags, result);
}


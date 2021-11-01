/*

Description: Hostname System
Author: Examplio
MY VK: ( vk.com/example_text )
Copyright (c) 2021 Viktor Kasko

*/

#if defined _hostnames_included_
    #endinput
#endif
#define _hostnames_included_

#tryinclude <a_samp>

#if !defined CMD
    #error ZCMD,DC_CMD or PAWN.CMD required by Hostname Plugin, please install it
#endif

#if defined H_VERSION
    #undef H_VERSION
#endif
#define H_VERSION   (1) // INC_ VERSION

#if __Pawn >= 0x3A && defined __PawnBuild
    #if __Pawn > 0x3A || __PawnBuild >= 2
        #define static_enum        static enum
    #endif
#endif

#if !defined static_enum
    #define static_enum            enum
#endif

#define MAX_HOSTNAME_LENGTH     80
#define MAX_TABLE_LENGTH        50

#define LOCAL_TABLE             "hostname_editor"
#define LOCAL_COLOR             "{FFCC00}"
#define LOCAL_WHITE_COLOR       "{FFFFFF}"

static
    static_SavedArray[MAX_PLAYERS][MAX_TABLE_LENGTH],
    static_string[MAX_TABLE_LENGTH * MAX_HOSTNAME_LENGTH + (MAX_TABLE_LENGTH * 8) + (MAX_TABLE_LENGTH * 1) + (12 + 8 + 16) + (MAX_TABLE_LENGTH * 4)];

static_enum {
    H_MAIN_PANEL,
    H_INPUT_PANEL,
    H_LIST_PANEL,
    H_PANEL
};

enum _HOSTNAME_DATA {
    H_NAME[MAX_HOSTNAME_LENGTH],
    H_IS_ACTIVATED,
    H_IS_MAIN
};

new 
    _H_DATA [ MAX_TABLE_LENGTH ] [ _HOSTNAME_DATA ] ;

public OnGameModeInit ( ) {
    #if defined hostname_OnGameModeInit
        hostname_OnGameModeInit ( ) ;
    #endif

    for(new i = 0; i < sizeof _H_DATA; i++) {
        _H_DATA [ i ] [ H_NAME ] [ 0 ] = EOS;
        _H_DATA [ i ] [ H_IS_ACTIVATED ] = 0;
        _H_DATA [ i ] [ H_IS_MAIN ] = 0;
    }

    print ( "-------------------------------------" ) ;
	print ( " Hostname System by Examplio loaded! " ) ;
	printf (" Version: %d ", H_VERSION ) ;
	print ( "-------------------------------------" ) ;
    
     #if !defined mysql_reconnect
        mysql_tquery ( MYSQL_DEFAULT_HANDLE , "CREATE TABLE IF NOT EXISTS `hostname_editor` (`ID` int(11) NOT NULL PRIMARY KEY AUTO_INCREMENT,`h_name` varchar(80) CHARACTER SET cp1251 NOT NULL DEFAULT 'hostname « Your Text »',`h_is_activated` int(11) NOT NULL DEFAULT '0',`h_is_main` int(11) NOT NULL DEFAULT '0') ENGINE=InnoDB DEFAULT CHARSET=cp1251;" , "" , "" ) ;
    #else
        #if defined cache_get_row_int
            mysql_tquery ( 1 , "CREATE TABLE IF NOT EXISTS `hostname_editor` (`ID` int(11) NOT NULL PRIMARY KEY AUTO_INCREMENT,`h_name` varchar(80) CHARACTER SET cp1251 NOT NULL DEFAULT 'hostname « Your Text »',`h_is_activated` int(11) NOT NULL DEFAULT '0',`h_is_main` int(11) NOT NULL DEFAULT '0') ENGINE=InnoDB DEFAULT CHARSET=cp1251;" , "" , "" ) ;
        #endif
    #endif

    #if !defined mysql_reconnect
        new
            Cache:qHandle = mysql_query( MYSQL_DEFAULT_HANDLE , "SELECT * FROM `"LOCAL_TABLE"`" ) ;

        if(!cache_num_rows ( ) )
            return cache_delete ( qHandle ) ;
        
        new
            static_count;

        static_count = cache_num_rows();
        
        for(new i = 0 ; i < static_count ; i++) {
            // String
            cache_get_value_name(i, "h_name", _H_DATA [ i ] [ H_NAME ] , MAX_HOSTNAME_LENGTH ) ;

            // int
            cache_get_value_name_int(i, !"h_is_activated", _H_DATA [ i ] [ H_IS_ACTIVATED ] ) ;
            cache_get_value_name_int(i, !"h_is_main", _H_DATA [ i ] [ H_IS_MAIN ] ) ;
        }

    #else
        #if defined cache_get_row_int
            new
                Cache:qHandle = mysql_query( 1 , "SELECT * FROM `"LOCAL_TABLE"`" ) ;
            
            if(!cache_num_rows( ) )
                return cache_delete( qHandle ) ;
            
            new
                static_count;

            static_count = cache_num_rows();
            
            for(new i = 0 ; i < static_count ; i++) {
                // String
                cache_get_field_content(i,"h_name" , _H_DATA [ i ] [ H_NAME ] , MAX_HOSTNAME_LENGTH, 1);

                // int
                _H_DATA [ i ] [ H_IS_ACTIVATED ] = cache_get_field_content_int (i,!"h_is_activated" ) ;
                _H_DATA [ i ] [ H_IS_MAIN ] = cache_get_field_content_int (i, !"h_is_main" ) ;
            }
        #endif
    #endif

    SetTimer("_Updating",1000,1);

    return cache_delete ( qHandle ) ;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {

    switch ( dialogid ) {
        case H_MAIN_PANEL: {

            if(!response)
                return true;
            
            switch(listitem) {
                case 0: return ShowPlayerDialog(playerid,H_INPUT_PANEL,DIALOG_STYLE_INPUT,!""LOCAL_COLOR"Новое название",!""LOCAL_WHITE_COLOR"Чтобы добавить новый хостнэйм, введите название которое в будущем будет отображаться в клиенте\n\nПример: Public Server 0.3.7.",!"Далее",!"Отмена");
                case 1: {
                    #if !defined mysql_reconnect
                        new
                            Cache:qHandle = mysql_query(MYSQL_DEFAULT_HANDLE,"SELECT * FROM `"LOCAL_TABLE"`");
                        
                        if(!cache_num_rows())
                            return SendClientMessage(playerid,-1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Нету ни одного добавленного хостнэйма."), cache_delete ( qHandle ), HostnamePanel(playerid);
                        
                        static
                            static_name[MAX_HOSTNAME_LENGTH],
                            static_Activated;

                        static_string = ""LOCAL_WHITE_COLOR"Название\t\t"LOCAL_WHITE_COLOR"Статус\n";

                        new
                            static_count = cache_num_rows();
                        
                        for ( new i = 0; i < static_count; i++ ) {

                            if(i > 49)
                                continue;

                            cache_get_value_name(i, !"h_name", static_name , MAX_HOSTNAME_LENGTH);

                            cache_get_value_name_int(i, !"h_is_activated", static_Activated);
                            cache_get_value_name_int(i, !"ID", static_SavedArray [ playerid ] [ i ] ) ;

                            format(static_string,sizeof(static_string),"\
                                %s%s\t\t%s\n",
                                static_string,
                                static_name,
                                0 == static_Activated ? "{FA8072}-" : "{00FF7F}+"
                            );
                        }
                    #else
                        #if defined cache_get_row_int
                            new
                                Cache:qHandle = mysql_query(1,"SELECT * FROM `"LOCAL_TABLE"`");

                            if(!cache_num_rows())
                                return SendClientMessage(playerid,-1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Нету ни одного добавленного хостнэйма."), cache_delete ( qHandle ) , HostnamePanel(playerid);
                            
                        
                            static
                                static_name[MAX_HOSTNAME_LENGTH],
                                static_Activated;

                            static_string = ""LOCAL_WHITE_COLOR"Название\t\t"LOCAL_WHITE_COLOR"Статус\n";

                            new
                                static_count = cache_num_rows();
                            
                            for ( new i = 0; i < static_count; i++ ) {

                                if(i > 49)
                                    continue;

                                cache_get_field_content(i, !"h_name", static_name , MAX_HOSTNAME_LENGTH);

                                static_Activated = cache_get_field_content_int(i, !"h_is_activated");
                                static_SavedArray [ playerid ] [ i ] = cache_get_field_content_int(i, !"ID") ;

                                format(static_string,sizeof(static_string),"\
                                    %s%s\t\t%s\n",
                                    static_string,
                                    static_name,
                                    0 == static_Activated ? "{FA8072}-" : "{00FF7F}+"
                                );
                            }
                        #endif
                    #endif

                    cache_delete ( qHandle ) ;

                    return ShowPlayerDialog(playerid,H_LIST_PANEL,DIALOG_STYLE_TABLIST_HEADERS,!""LOCAL_COLOR"Панель управления",static_string,!"Далее",!"Назад");
                }
            }
        }
        case H_LIST_PANEL: {
            if(!response)
                return HostnamePanel ( playerid ) ;
    
            SetPVarInt(playerid,!"_IDReceived",listitem);
            return ShowPlayerDialog(playerid,H_PANEL,DIALOG_STYLE_LIST,!""LOCAL_COLOR"Панель",!"{00FF7F}* "LOCAL_WHITE_COLOR"Включить\n{FA8072}* "LOCAL_WHITE_COLOR"Выключить\n{ff0000}* "LOCAL_WHITE_COLOR"Удалить",!"Далее",!"Отмена");
        }
        case H_PANEL: {
            if(!response)
                return HostnamePanel ( playerid );
            
            static
                _data,
                query_string[67 + 15 + 2];
            
            _data = GetPVarInt(playerid,!"_IDReceived");

            _data = static_SavedArray [ playerid ] [ _data ];

            switch(listitem) {
                case 0: {
                    format(query_string,sizeof(query_string),"\
                        UPDATE `"LOCAL_TABLE"` SET `h_is_activated` = '1' WHERE `ID` = '%i'",
                        _data
                    );
                    
                     #if !defined mysql_reconnect
                        mysql_tquery(MYSQL_DEFAULT_HANDLE,query_string,"","");

                    #else
                        #if defined cache_get_row_int
                            mysql_tquery(1,query_string,"","");
                        #endif
                    #endif

                    SendClientMessage(playerid,-1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Успех!");

                    e_UpdateInfo();

                    return HostnamePanel ( playerid );
                }
                case 1: {
                    format(query_string,sizeof(query_string),"\
                        UPDATE `"LOCAL_TABLE"` SET `h_is_activated` = '0' WHERE `ID` = '%i'",
                        _data
                    );
                    
                     #if !defined mysql_reconnect
                        mysql_tquery(MYSQL_DEFAULT_HANDLE,query_string,"","");

                    #else
                        #if defined cache_get_row_int
                            mysql_tquery(1,query_string,"","");
                        #endif
                    #endif

                    SendClientMessage(playerid,-1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Успех!");

                    e_UpdateInfo();

                    return HostnamePanel ( playerid );
                }
                case 2: {
                    format(query_string,sizeof(query_string),"\
                        DELETE FROM `"LOCAL_TABLE"` WHERE `ID` = '%i'",
                        _data
                    );
                    
                     #if !defined mysql_reconnect
                        mysql_tquery(MYSQL_DEFAULT_HANDLE,query_string,"","");
                    #else
                        #if defined cache_get_row_int
                            mysql_tquery(1,query_string,"","");
                        #endif
                    #endif

                    SendClientMessage(playerid,-1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Успех!");

                    e_UpdateInfo();

                    return HostnamePanel ( playerid );
                }
            }
        }
        case H_INPUT_PANEL: {
            if(!response)
                return HostnamePanel ( playerid );
            
            if(strlen(inputtext) > MAX_HOSTNAME_LENGTH)
                return ShowPlayerDialog(playerid,H_INPUT_PANEL,DIALOG_STYLE_INPUT,!""LOCAL_COLOR"Новое название",!""LOCAL_WHITE_COLOR"Чтобы добавить новый хостнэйм, введите название которое в будущем будет отображаться в клиенте\n\nПример: Public Server 0.3.7.\n\n{ff0000}Макс. длина – 100",!"Далее",!"Отмена");
            
            #if !defined mysql_reconnect
                static
                    query_string[53 + 15 + MAX_HOSTNAME_LENGTH];
                
                mysql_format(MYSQL_DEFAULT_HANDLE,query_string,sizeof(query_string),"\
                    INSERT INTO `"LOCAL_TABLE"` (`h_name`) VALUES ('%e')",
                    inputtext
                );
                mysql_tquery(MYSQL_DEFAULT_HANDLE,query_string,"","");

            #else
                #if defined cache_get_row_int
                    static
                        query_string[53 + 15 + MAX_HOSTNAME_LENGTH];
                
                    mysql_format(1,query_string,sizeof(query_string),"\
                        INSERT INTO `"LOCAL_TABLE"` (`h_name`) VALUES ('%e')",
                        inputtext
                    );
                    mysql_tquery(1,query_string,"","");

                #endif
            #endif

            e_UpdateInfo();

            SendClientMessage(playerid, -1,!""LOCAL_COLOR"Info | "LOCAL_WHITE_COLOR"Вы успешно добавили новое название!");

            return HostnamePanel ( playerid ) ;
        }
    }
    #if defined hostname_OnDialogResponse
        hostname_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
    #endif

    return 1;
}

// Forwards
forward hostname_OnGameModeInit ( ) ;
forward hostname_OnDialogResponse( playerid , dialogid , response , listitem , inputtext [ ] ) ;
// Other
new
    static_numeric = 0;

static
    f_string[MAX_HOSTNAME_LENGTH + 9];

forward _Updating();
public _Updating() {

    for(new i = 0 ; i < MAX_TABLE_LENGTH; i++) {
        if(!_H_DATA[i][H_IS_ACTIVATED])
            continue;
        
        static_numeric++;
    }
    if(!static_numeric)
        return true;
    
    static_numeric = random(static_numeric);

    format(f_string,sizeof(f_string),"hostname %s",_H_DATA[static_numeric][H_NAME]);

    static_numeric = 0;
    
    SendRconCommand(f_string);
    return 1;
}
// Stocks
stock HostnamePanel ( playerid ) {
    return ShowPlayerDialog(playerid,H_MAIN_PANEL,DIALOG_STYLE_LIST,!""LOCAL_COLOR"Панель названий",!""LOCAL_COLOR"1. "LOCAL_WHITE_COLOR"Добавить новое название\n"LOCAL_COLOR"2. "LOCAL_WHITE_COLOR"Панель управления",!"Далее",!"Отмена");
}
stock e_UpdateInfo() {
    for(new i = 0; i < sizeof _H_DATA; i++) {
        _H_DATA [ i ] [ H_NAME ] [ 0 ] = EOS;
        _H_DATA [ i ] [ H_IS_ACTIVATED ] = 0;
        _H_DATA [ i ] [ H_IS_MAIN ] = 0;
    }

    #if !defined mysql_reconnect
        new
            Cache:qHandle = mysql_query(MYSQL_DEFAULT_HANDLE,"SELECT * FROM `"LOCAL_TABLE"` WHERE `h_is_activated` = '1'");
        
        if(!cache_num_rows())
            return cache_delete(qHandle);
        
        new
            static_count;

        static_count = cache_num_rows();
        
        for(new i = 0 ; i < static_count ; i++) {
            if(i > 49)
                continue;

            // String
            cache_get_value_name(i, !"h_name", _H_DATA [ i ] [ H_NAME ] , MAX_HOSTNAME_LENGTH ) ;

            // int
            cache_get_value_name_int(i, !"h_is_activated", _H_DATA [ i ] [ H_IS_ACTIVATED ] ) ;
            cache_get_value_name_int(i, !"h_is_main", _H_DATA [ i ] [ H_IS_MAIN ] ) ;
        }
        
    #else
        #if defined cache_get_row_int
            new
                Cache:qHandle = mysql_query(1,"SELECT * FROM `"LOCAL_TABLE"` WHERE `h_is_activated` = '1'");
        
            if(!cache_num_rows())
                return cache_delete(qHandle);
            
            new
                static_count;

            static_count = cache_num_rows();

            for(new i = 0 ; i < static_count ; i++) {
                if(i > 49)
                    continue;

                // String
                cache_get_field_content(i,!"h_name" , _H_DATA [ i ] [ H_NAME ] , MAX_HOSTNAME_LENGTH, 1);

                // int
                _H_DATA [ i ] [ H_IS_ACTIVATED ] = cache_get_field_content_int (i,!"h_is_activated" ) ;
                _H_DATA [ i ] [ H_IS_MAIN ] = cache_get_field_content_int (i, !"h_is_main" ) ;
            }
        #endif
    #endif
    
    cache_delete(qHandle);
    return 1;
}

#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif

#define OnDialogResponse hostname_OnDialogResponse

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif

#define OnGameModeInit hostname_OnGameModeInit

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif

#define OnGameModeInit hostname_OnGameModeInit
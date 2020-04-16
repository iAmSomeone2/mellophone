/*
    Copyright 2020 Brenden Davidson

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

extern "C"{
    static const char SQLITE_INIT_STMT[] = 
    "BEGIN TRANSACTION;" \
    "CREATE TABLE \"artists\" (" \
    "\"id\"	    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE," \
    "\"name\"	    TEXT NOT NULL UNIQUE," \
    "\"picture\"   INTEGER," \
    "FOREIGN KEY(\"picture\") REFERENCES \"artist_pics\"(\"id\")" \
    ");" \
    "CREATE TABLE \"artist_pics\" (" \
    "\"id\"            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE," \
    "\"file_location\" TEXT NOT NULL" \
    ");" \
    "CREATE TABLE \"albums\" (" \
    "\"id\"	    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE," \
    "\"name\"	    TEXT NOT NULL," \
    "\"artist\"	INTEGER NOT NULL," \
    "\"artwork\"	INTEGER," \
    "FOREIGN KEY(\"artist\") REFERENCES \"artists\"(\"id\")," \
    "FOREIGN KEY(\"artwork\") REFERENCES \"album_art\"(\"id\")" \
    ");" \
    "CREATE TABLE \"album_art\" (" \
    "\"id\"            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE," \
    "\"file_location\" TEXT NOT NULL" \
    ");" \
    "CREATE TABLE \"songs\" (" \
    "\"id\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE," \
    "\"name\"	TEXT NOT NULL," \
    "\"album\"	INTEGER NOT NULL," \
    "\"track_number\"	INTEGER NOT NULL," \
    "\"file_location\"	TEXT NOT NULL UNIQUE," \
    "\"artwork\"	BLOB," \
    "FOREIGN KEY(\"album\") REFERENCES \"albums\"(\"id\")," \
    "FOREIGN KEY(\"artwork\") REFERENCES \"albums\"(\"artwork\")" \
    ");" \
    "COMMIT;";

    static const int INIT_STMT_LEN = 1026;
};

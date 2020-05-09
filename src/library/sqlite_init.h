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

extern "C"
{
    static const char SQLITE_INIT_STMT[] =
        "BEGIN TRANSACTION;"
        "CREATE TABLE \"Albums\" ("
        "\"ID\"	INTEGER NOT NULL UNIQUE,"
        "\"Name\"	TEXT NOT NULL,"
        "\"Artist\"	INTEGER NOT NULL,"
        "\"CoverArt\"	BLOB,"
        "PRIMARY KEY(\"ID\"),"
        "FOREIGN KEY(\"Artist\") REFERENCES \"Artists\"(\"ID\")"
        "ON UPDATE CASCADE "
        "ON DELETE CASCADE"
        ");"
        "CREATE TABLE \"Artists\" ("
        "\"ID\"	INTEGER NOT NULL UNIQUE,"
        "\"Name\"	TEXT NOT NULL UNIQUE,"
        "\"Picture\"	BLOB,"
        "PRIMARY KEY(\"ID\")"
        ");"
        "CREATE TABLE \"Tracks\" ("
        "\"Checksum\"	TEXT NOT NULL UNIQUE,"
        "\"FileLocation\"	TEXT NOT NULL UNIQUE,"
        "\"Title\"	TEXT NOT NULL,"
        "\"Album\"	INTEGER NOT NULL,"
        "\"TrackNum\"	INTEGER,"
        "\"TotalTracks\"	INTEGER,"
        "\"DiscNum\"	INTEGER,"
        "\"TotalDiscs\"	INTEGER,"
        "PRIMARY KEY(\"Checksum\"),"
        "FOREIGN KEY(\"Album\") REFERENCES \"Albums\"(\"ID\")"
        "ON UPDATE CASCADE "
        "ON DELETE CASCADE"
        ");"
        "COMMIT;";

    static const int INIT_STMT_LEN = 682;
};

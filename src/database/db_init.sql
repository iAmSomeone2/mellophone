BEGIN TRANSACTION;

CREATE TABLE "artists" (
	"id"	    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	"name"	    TEXT NOT NULL UNIQUE,
    "picture"   INTEGER,
    FOREIGN KEY("picture") REFERENCES "artist_pics"("id")
);

CREATE TABLE "artist_pics" (
    "id"            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    "file_location" TEXT NOT NULL
);

CREATE TABLE "albums" (
	"id"	    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	"name"	    TEXT NOT NULL,
	"artist"	INTEGER NOT NULL,
	"artwork"	INTEGER,
	FOREIGN KEY("artist") REFERENCES "artists"("id"),
    FOREIGN KEY("artwork") REFERENCES "album_art"("id")
);

CREATE TABLE "album_art" (
    "id"            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
    "file_location" TEXT NOT NULL
);

CREATE TABLE "songs" (
	"id"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
	"name"	TEXT NOT NULL,
	"album"	INTEGER NOT NULL,
	"track_number"	INTEGER NOT NULL,
	"file_location"	TEXT NOT NULL UNIQUE,
	"artwork"	BLOB,
	FOREIGN KEY("album") REFERENCES "albums"("id"),
	FOREIGN KEY("artwork") REFERENCES "albums"("artwork")
);

COMMIT;
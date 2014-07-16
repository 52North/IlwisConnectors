
1. Install prerequisites (PostgreSQL with PostGIS and PGAdmin III). For [windows there is a stack builder](http://postgis.net/windows_downloads), otherwise check [the install section](http://postgis.net/install) of PostGIS.
1. [Download the test data ](https://drive.google.com/?pli=1&authuser=0&urp=https://docs.google.com/?pli%3D1%26authuser%3D0#folders/0B8n9lfkrXoqScnVFRmF5blFSSVU)
1. Create database with name 'ilwis-pg-test', should be like 
```
CREATE DATABASE "ilwis-pg-test"
  WITH OWNER = postgres
	   ENCODING = 'UTF8'
	   TABLESPACE = pg_default
	   LC_COLLATE = 'German, Germany'
	   LC_CTYPE = 'German, Germany'
	   CONNECTION LIMIT = -1;
```
1. Import shape files via PGAdmin III tool.
1. In Import Dialog edit the srid and set it to 4269 (double check via qGIS, or your favorite tool)
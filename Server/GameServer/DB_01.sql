CREATE TABLE [dbo].[Version]
(
	[version] FLOAT NOT NULL
);

CREATE TABLE [dbo].[Golad]
(
	[id] INT NOT NULL PRIMARY KEY IDENTITY,
	[gold] INT NULL,
	[name] NVARCHAR(50) NULL,
	[createData] DATETIME NULL
);
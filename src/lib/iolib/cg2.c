/* Character generator table -- 12V by 8H */

int	cg_rows = 12;

int	cgtable[12][256] = {

	{	/* Scan line 0 */
	 0x0000, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x0088, 0x006E, 0x000E, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0010, 0x0000, 0x0000, 0x0000, 
	 0x0020, 0x0004, 0x0000, 0x0000, 0x0004, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0004, 0x0040, 0x0000, 0x0002, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0078, 0x0000, 0x001E, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x003C, 
	 0x0000, 0x0000, 0x001C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0002, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0006, 0x0000, 0x0030, 0x0010, 0x000C, 0x0000, 0x0000, 
	 0x0050, 0x003A, 0x00A9, 0x008B, 0x0000, 0x0000, 0x00E2, 0x00EE, 
	 0x00DC, 0x005D, 0x0049, 0x0097, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x00EE, 0x00AE, 0x00EE, 0x0000, 0x0040, 0x00EA, 0x00EE, 
	 0x0000, 0x00E2, 0x00AE, 0x00E4, 0x0000, 0x0094, 0x00E0, 0x004E, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0004, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0024, 
	 0x0000, 0x0000, 0x0024, 0x0024, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 1 */
	 0x0000, 0x0000, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x0088, 0x00AA, 0x0002, 
	 0x0018, 0x007E, 0x007E, 0x003C, 0x0060, 0x003C, 0x003C, 0x000C, 
	 0x003C, 0x001C, 0x0000, 0x0000, 0x0073, 0x0066, 0x00FB, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x0000, 0x007C, 0x0040, 0x00BC, 0x0000, 
	 0x0010, 0x0008, 0x0010, 0x0000, 0x0008, 0x0000, 0x0018, 0x0000, 
	 0x0018, 0x007C, 0x007E, 0x003C, 0x0020, 0x003C, 0x003C, 0x0004, 
	 0x003C, 0x001C, 0x0000, 0x0008, 0x0020, 0x0000, 0x0004, 0x0008, 
	 0x0078, 0x0042, 0x003E, 0x003C, 0x003E, 0x007E, 0x0004, 0x003C, 
	 0x0042, 0x007C, 0x001C, 0x0082, 0x007E, 0x0082, 0x0042, 0x003C, 
	 0x0002, 0x005C, 0x0042, 0x003C, 0x0038, 0x003C, 0x0010, 0x0044, 
	 0x00C6, 0x0038, 0x007E, 0x0008, 0x0000, 0x0010, 0x0000, 0x00FE, 
	 0x0000, 0x00BC, 0x001A, 0x0038, 0x0058, 0x0038, 0x0008, 0x0042, 
	 0x0042, 0x0038, 0x0022, 0x0042, 0x0038, 0x0092, 0x0042, 0x0018, 
	 0x0002, 0x0040, 0x0002, 0x0038, 0x0030, 0x0058, 0x0010, 0x006C, 
	 0x0082, 0x0008, 0x007C, 0x0008, 0x0010, 0x0010, 0x0000, 0x003C, 
	 0x0050, 0x0022, 0x00A9, 0x008A, 0x0000, 0x0000, 0x00A2, 0x0028, 
	 0x0044, 0x0045, 0x0049, 0x0095, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x00A8, 0x00AA, 0x0022, 0x0000, 0x00A0, 0x002A, 0x002A, 
	 0x0000, 0x00A2, 0x00A2, 0x0024, 0x0000, 0x0094, 0x00A2, 0x004A, 
	 0x00E0, 0x00E0, 0x00C0, 0x00C0, 0x00DF, 0x0000, 0x00DF, 0x0000, 
	 0x0024, 0x000C, 0x0010, 0x0010, 0x0022, 0x00EE, 0x00FA, 0x0000, 
	 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0024, 
	 0x0000, 0x0000, 0x0024, 0x0024, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x000C, 0x000E, 0x001E, 0x000C, 0x0008, 0x000C, 0x000C, 0x0002, 
	 0x000C, 0x000C, 0x0032, 0x0077, 0x00F7, 0x0000, 0x0000, 0x0000, 
	 0x0030, 0x0070, 0x0078, 0x0030, 0x0076, 0x007B, 0x0033, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 2 */
	 0x0000, 0x0000, 0x0000, 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x00A8, 0x00AA, 0x0006, 
	 0x003C, 0x007E, 0x007E, 0x007E, 0x0060, 0x007E, 0x007E, 0x000C, 
	 0x007E, 0x003C, 0x0000, 0x0000, 0x00FB, 0x0066, 0x00FB, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0028, 0x0092, 0x00A2, 0x0042, 0x0000, 
	 0x0010, 0x0008, 0x0092, 0x0010, 0x0018, 0x0000, 0x0018, 0x0002, 
	 0x0024, 0x0010, 0x0042, 0x0042, 0x0020, 0x0042, 0x0042, 0x0004, 
	 0x0042, 0x0020, 0x0018, 0x0018, 0x0010, 0x0000, 0x0008, 0x0000, 
	 0x0004, 0x0042, 0x0044, 0x0042, 0x0044, 0x0044, 0x0004, 0x0042, 
	 0x0042, 0x0010, 0x0022, 0x0042, 0x0042, 0x0082, 0x0062, 0x0042, 
	 0x0002, 0x0022, 0x0022, 0x0042, 0x0010, 0x0042, 0x0010, 0x00C6, 
	 0x0044, 0x0010, 0x0042, 0x0008, 0x0080, 0x0010, 0x0000, 0x0000, 
	 0x0000, 0x0042, 0x0026, 0x0044, 0x0064, 0x0044, 0x0008, 0x0058, 
	 0x0042, 0x0010, 0x0022, 0x0022, 0x0010, 0x0092, 0x0042, 0x0024, 
	 0x001A, 0x0058, 0x0002, 0x0044, 0x0048, 0x0064, 0x0028, 0x0092, 
	 0x0044, 0x0010, 0x0004, 0x0008, 0x0010, 0x0010, 0x0000, 0x0042, 
	 0x0050, 0x003B, 0x0099, 0x00AA, 0x0000, 0x0000, 0x00AE, 0x006E, 
	 0x0044, 0x00C5, 0x0049, 0x00D5, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x00AE, 0x006A, 0x0062, 0x0000, 0x00A0, 0x002E, 0x006A, 
	 0x0000, 0x00AE, 0x00E2, 0x0064, 0x0000, 0x00D4, 0x00A4, 0x004A, 
	 0x0040, 0x0040, 0x00C0, 0x00C0, 0x00C4, 0x0000, 0x00DF, 0x0000, 
	 0x0026, 0x0014, 0x0038, 0x0010, 0x0052, 0x0044, 0x000A, 0x0000, 
	 0x0024, 0x0000, 0x0066, 0x0000, 0x0026, 0x0000, 0x0064, 0x0066, 
	 0x0000, 0x0000, 0x0064, 0x0026, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0004, 0x0002, 0x0012, 0x0008, 0x0012, 0x0012, 0x0002, 
	 0x0012, 0x0010, 0x004A, 0x0022, 0x0012, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0020, 0x0008, 0x0048, 0x0026, 0x000B, 0x004B, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 3 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 0x007E, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x00D8, 0x00AA, 0x0002, 
	 0x007E, 0x0018, 0x0066, 0x0066, 0x0060, 0x0066, 0x0066, 0x0018, 
	 0x0066, 0x0070, 0x0000, 0x0018, 0x00DB, 0x0066, 0x001B, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0028, 0x0090, 0x0044, 0x00A2, 0x0000, 
	 0x0008, 0x0010, 0x0054, 0x0010, 0x0018, 0x0000, 0x0000, 0x0004, 
	 0x0042, 0x0010, 0x0002, 0x0040, 0x0020, 0x0040, 0x0042, 0x0008, 
	 0x0042, 0x0040, 0x0018, 0x0018, 0x0008, 0x0000, 0x0010, 0x0008, 
	 0x0032, 0x0042, 0x0044, 0x0002, 0x0044, 0x0004, 0x0004, 0x0042, 
	 0x0042, 0x0010, 0x0022, 0x0022, 0x0002, 0x0082, 0x0062, 0x0042, 
	 0x0002, 0x0052, 0x0012, 0x0040, 0x0010, 0x0042, 0x0028, 0x00AA, 
	 0x0028, 0x0010, 0x0004, 0x0008, 0x0040, 0x0010, 0x0000, 0x0000, 
	 0x0000, 0x0042, 0x0042, 0x0002, 0x0042, 0x0002, 0x0008, 0x0064, 
	 0x0042, 0x0010, 0x0020, 0x0016, 0x0010, 0x0092, 0x0042, 0x0042, 
	 0x0026, 0x0064, 0x0002, 0x0040, 0x0008, 0x0042, 0x0028, 0x0092, 
	 0x0028, 0x0028, 0x0008, 0x0008, 0x0010, 0x0010, 0x0000, 0x0099, 
	 0x00D0, 0x000A, 0x00A9, 0x00DA, 0x0000, 0x0000, 0x00AA, 0x0022, 
	 0x0044, 0x0045, 0x0049, 0x00B5, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x00A2, 0x00AA, 0x0022, 0x0000, 0x00A0, 0x002A, 0x002A, 
	 0x0000, 0x00AA, 0x00A2, 0x0024, 0x0000, 0x00B4, 0x00A8, 0x004A, 
	 0x0044, 0x0040, 0x00CC, 0x00C0, 0x0004, 0x0000, 0x000C, 0x0000, 
	 0x003C, 0x0024, 0x007C, 0x0010, 0x008A, 0x0044, 0x000A, 0x0000, 
	 0x0024, 0x0000, 0x00C3, 0x0000, 0x0023, 0x0000, 0x00C4, 0x00C3, 
	 0x0000, 0x0000, 0x00C4, 0x0023, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0004, 0x0002, 0x0010, 0x0008, 0x0010, 0x0012, 0x0004, 
	 0x0012, 0x0010, 0x004A, 0x0022, 0x0022, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0020, 0x0008, 0x0040, 0x0020, 0x0008, 0x0040, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 4 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x0088, 0x006E, 0x000E, 
	 0x0066, 0x0018, 0x0006, 0x0060, 0x00FE, 0x0060, 0x0066, 0x0018, 
	 0x0066, 0x0060, 0x0000, 0x0018, 0x00DB, 0x0066, 0x0033, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x00FE, 0x0090, 0x0008, 0x0014, 0x0000, 
	 0x0008, 0x0010, 0x0038, 0x0010, 0x0000, 0x0000, 0x0000, 0x0008, 
	 0x0042, 0x0010, 0x0004, 0x0040, 0x0020, 0x0040, 0x0042, 0x0008, 
	 0x0042, 0x0040, 0x0000, 0x0000, 0x0004, 0x003C, 0x0020, 0x0008, 
	 0x004A, 0x007E, 0x0044, 0x0002, 0x0044, 0x0004, 0x0004, 0x0042, 
	 0x0042, 0x0010, 0x0020, 0x0012, 0x0002, 0x0092, 0x0052, 0x0042, 
	 0x0002, 0x0042, 0x000A, 0x0040, 0x0010, 0x0042, 0x0028, 0x00AA, 
	 0x0028, 0x0010, 0x0004, 0x0008, 0x0020, 0x0010, 0x0000, 0x0000, 
	 0x0000, 0x004C, 0x0042, 0x0002, 0x0042, 0x003E, 0x0008, 0x0042, 
	 0x0042, 0x0010, 0x0020, 0x000A, 0x0010, 0x0092, 0x0042, 0x0042, 
	 0x0042, 0x0042, 0x0002, 0x0038, 0x0008, 0x0042, 0x0044, 0x0092, 
	 0x0010, 0x0024, 0x0010, 0x0008, 0x0010, 0x0010, 0x0060, 0x00A5, 
	 0x0050, 0x00BA, 0x00BB, 0x008A, 0x0000, 0x0000, 0x00EE, 0x00EE, 
	 0x00C4, 0x009D, 0x005C, 0x0097, 0x0072, 0x0077, 0x0097, 0x0008, 
	 0x0000, 0x00EE, 0x00EA, 0x00EE, 0x0000, 0x00A0, 0x0024, 0x00EA, 
	 0x0000, 0x00EE, 0x0042, 0x00EE, 0x0000, 0x0094, 0x00E0, 0x00EA, 
	 0x0044, 0x0040, 0x00DE, 0x00DF, 0x0004, 0x0000, 0x000C, 0x0000, 
	 0x0064, 0x0044, 0x0092, 0x0010, 0x008A, 0x0044, 0x0012, 0x0000, 
	 0x0024, 0x00FF, 0x0081, 0x0000, 0x0021, 0x00FF, 0x0084, 0x0081, 
	 0x0007, 0x00E0, 0x008C, 0x0031, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0004, 0x0004, 0x0010, 0x001F, 0x0010, 0x0012, 0x0004, 
	 0x0012, 0x001C, 0x004A, 0x0022, 0x0042, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0020, 0x0010, 0x0040, 0x0020, 0x0010, 0x0040, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 5 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x0000, 0x0000, 0x0000, 
	 0x0066, 0x0018, 0x000C, 0x0038, 0x00FE, 0x007E, 0x007E, 0x0030, 
	 0x003C, 0x007C, 0x007C, 0x007E, 0x00DB, 0x0066, 0x0063, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x0028, 0x007C, 0x0010, 0x0008, 0x0000, 
	 0x0008, 0x0010, 0x00FE, 0x00FE, 0x0000, 0x007C, 0x0000, 0x0010, 
	 0x0042, 0x0010, 0x0008, 0x0040, 0x00FE, 0x0040, 0x0046, 0x0010, 
	 0x0042, 0x007C, 0x0000, 0x0000, 0x0002, 0x0000, 0x0040, 0x0010, 
	 0x009A, 0x0042, 0x0044, 0x0002, 0x0044, 0x0004, 0x0004, 0x0072, 
	 0x0042, 0x0010, 0x0020, 0x000A, 0x0002, 0x0092, 0x0052, 0x0042, 
	 0x003E, 0x0042, 0x003E, 0x0040, 0x0010, 0x0042, 0x0044, 0x0092, 
	 0x0010, 0x0010, 0x0008, 0x0008, 0x0010, 0x0010, 0x0000, 0x0000, 
	 0x0000, 0x0070, 0x0042, 0x0002, 0x0042, 0x0042, 0x003C, 0x0042, 
	 0x0042, 0x0010, 0x0020, 0x0012, 0x0010, 0x0092, 0x0042, 0x0042, 
	 0x0042, 0x0042, 0x0006, 0x0004, 0x0008, 0x0042, 0x0044, 0x0092, 
	 0x0028, 0x0044, 0x0020, 0x0004, 0x0000, 0x0020, 0x0092, 0x0085, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0015, 0x0051, 0x0091, 0x0008, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x005F, 0x005F, 0x00DE, 0x00DF, 0x0004, 0x0000, 0x000C, 0x0000, 
	 0x0024, 0x004C, 0x0010, 0x0010, 0x008A, 0x0044, 0x0022, 0x0000, 
	 0x0024, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0004, 0x0000, 
	 0x000C, 0x0030, 0x0018, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0004, 0x0008, 0x000C, 0x0009, 0x000E, 0x000E, 0x0008, 
	 0x000C, 0x0012, 0x004A, 0x0022, 0x0082, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0020, 0x0020, 0x0030, 0x0020, 0x0020, 0x0038, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 6 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x00A8, 0x003B, 0x0000, 
	 0x0066, 0x0018, 0x0018, 0x0038, 0x0066, 0x003E, 0x003E, 0x0030, 
	 0x003C, 0x007E, 0x007C, 0x007E, 0x00DB, 0x0066, 0x00C3, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x0028, 0x0012, 0x0020, 0x0014, 0x0000, 
	 0x0008, 0x0010, 0x0038, 0x0010, 0x0000, 0x0000, 0x0000, 0x0020, 
	 0x0042, 0x0010, 0x0010, 0x0038, 0x0022, 0x003E, 0x003A, 0x0010, 
	 0x003C, 0x0042, 0x0018, 0x0018, 0x0004, 0x003C, 0x0020, 0x0020, 
	 0x00AA, 0x0042, 0x003C, 0x0002, 0x0044, 0x001C, 0x001C, 0x0002, 
	 0x007E, 0x0010, 0x0020, 0x0006, 0x0002, 0x00AA, 0x004A, 0x0042, 
	 0x0042, 0x0042, 0x0042, 0x003C, 0x0010, 0x0042, 0x0044, 0x0092, 
	 0x0010, 0x0028, 0x0010, 0x0008, 0x0008, 0x0010, 0x0082, 0x0000, 
	 0x0000, 0x0042, 0x0026, 0x0044, 0x0064, 0x0044, 0x0008, 0x0024, 
	 0x0026, 0x0010, 0x0020, 0x0022, 0x0010, 0x0092, 0x0026, 0x0024, 
	 0x0026, 0x0064, 0x004A, 0x0044, 0x0008, 0x0042, 0x0082, 0x0082, 
	 0x0044, 0x0042, 0x0040, 0x0008, 0x0010, 0x0010, 0x000C, 0x0085, 
	 0x00AF, 0x00EE, 0x0002, 0x0000, 0x0052, 0x0095, 0x000E, 0x0000, 
	 0x0023, 0x00A9, 0x00A2, 0x000E, 0x0035, 0x0051, 0x0091, 0x0014, 
	 0x0095, 0x00EA, 0x00EE, 0x0001, 0x0075, 0x0077, 0x0027, 0x0009, 
	 0x0025, 0x00B9, 0x0002, 0x0000, 0x0071, 0x00E9, 0x000A, 0x0000, 
	 0x0044, 0x0040, 0x00CC, 0x00C0, 0x0004, 0x0000, 0x000C, 0x0000, 
	 0x0026, 0x0034, 0x0010, 0x0010, 0x008A, 0x0044, 0x0042, 0x0000, 
	 0x0024, 0x0000, 0x0000, 0x00FF, 0x0020, 0x0000, 0x0004, 0x0000, 
	 0x0018, 0x0018, 0x0030, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0004, 0x0010, 0x0010, 0x000A, 0x0002, 0x0002, 0x0008, 
	 0x0012, 0x0012, 0x004A, 0x0022, 0x0082, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0020, 0x0040, 0x0040, 0x0020, 0x0040, 0x0008, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 7 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x007E, 0x007E, 0x007E, 0x007E, 0x007E, 0x00A8, 0x0008, 0x0000, 
	 0x0066, 0x001E, 0x0030, 0x0060, 0x006C, 0x0006, 0x0006, 0x0060, 
	 0x0066, 0x0066, 0x0000, 0x0018, 0x00DB, 0x0066, 0x00C3, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x00FE, 0x0012, 0x0044, 0x0022, 0x0000, 
	 0x0008, 0x0010, 0x0054, 0x0010, 0x0000, 0x0000, 0x0000, 0x0040, 
	 0x0042, 0x0010, 0x0020, 0x0040, 0x0024, 0x0002, 0x0002, 0x0020, 
	 0x0042, 0x0042, 0x0018, 0x0018, 0x0008, 0x0000, 0x0010, 0x0040, 
	 0x00AA, 0x0042, 0x0044, 0x0002, 0x0044, 0x0004, 0x0004, 0x0002, 
	 0x0042, 0x0010, 0x0020, 0x000A, 0x0002, 0x00AA, 0x004A, 0x0042, 
	 0x0042, 0x0042, 0x0042, 0x0002, 0x0010, 0x0042, 0x0044, 0x0082, 
	 0x0028, 0x0044, 0x0020, 0x0008, 0x0004, 0x0010, 0x0044, 0x0000, 
	 0x0000, 0x003C, 0x001A, 0x0038, 0x0058, 0x0038, 0x0008, 0x0018, 
	 0x001A, 0x0018, 0x0030, 0x0042, 0x0010, 0x006E, 0x001A, 0x0018, 
	 0x001A, 0x0058, 0x0032, 0x0038, 0x003E, 0x0042, 0x0082, 0x0082, 
	 0x0082, 0x0042, 0x007C, 0x0008, 0x0010, 0x0010, 0x0000, 0x00A5, 
	 0x00A9, 0x00AA, 0x0022, 0x0000, 0x0052, 0x0095, 0x0008, 0x0000, 
	 0x0025, 0x00A9, 0x00A2, 0x0022, 0x0015, 0x0051, 0x0091, 0x0014, 
	 0x0095, 0x002A, 0x002A, 0x0001, 0x0015, 0x0054, 0x0021, 0x0029, 
	 0x0025, 0x0089, 0x0002, 0x0000, 0x0051, 0x0029, 0x000A, 0x0000, 
	 0x0044, 0x0040, 0x00C0, 0x00C0, 0x0004, 0x0000, 0x000F, 0x0000, 
	 0x003C, 0x0004, 0x0010, 0x0092, 0x008A, 0x0044, 0x0082, 0x0000, 
	 0x0024, 0x00FF, 0x0081, 0x0000, 0x0021, 0x0081, 0x0084, 0x00FF, 
	 0x0031, 0x008C, 0x00E0, 0x0007, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0012, 0x0006, 0x0012, 0x0012, 0x000C, 0x0002, 0x0002, 0x0010, 
	 0x0012, 0x0012, 0x004B, 0x0033, 0x0093, 0x0000, 0x0000, 0x0000, 
	 0x0048, 0x0030, 0x0048, 0x0048, 0x0030, 0x0048, 0x0008, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 8 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x007E, 0x007E, 0x007E, 0x007E, 0x0098, 0x0009, 0x0000, 
	 0x007E, 0x001C, 0x0066, 0x0066, 0x0078, 0x0006, 0x000E, 0x0066, 
	 0x0066, 0x0066, 0x0000, 0x0018, 0x00DB, 0x0077, 0x00DB, 0x0000, 
	 0x0000, 0x0010, 0x0000, 0x0028, 0x0092, 0x008A, 0x0022, 0x0004, 
	 0x0008, 0x0010, 0x0092, 0x0010, 0x0000, 0x0000, 0x0000, 0x0080, 
	 0x0042, 0x0014, 0x0040, 0x0040, 0x0028, 0x0002, 0x0002, 0x0020, 
	 0x0042, 0x0042, 0x0000, 0x0000, 0x0010, 0x0000, 0x0008, 0x0042, 
	 0x0092, 0x0024, 0x0044, 0x0002, 0x0044, 0x0004, 0x0004, 0x0002, 
	 0x0042, 0x0010, 0x0020, 0x0012, 0x0002, 0x00C6, 0x0046, 0x0042, 
	 0x0042, 0x0042, 0x0042, 0x0002, 0x0010, 0x0042, 0x0082, 0x0082, 
	 0x0028, 0x0044, 0x0020, 0x0008, 0x0002, 0x0010, 0x0028, 0x0000, 
	 0x0020, 0x0000, 0x0002, 0x0000, 0x0040, 0x0000, 0x0048, 0x0000, 
	 0x0002, 0x0000, 0x0000, 0x0002, 0x0010, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0008, 0x0010, 0x0010, 0x0000, 0x0099, 
	 0x006D, 0x00AA, 0x004E, 0x0000, 0x0032, 0x00D7, 0x00EE, 0x0000, 
	 0x0055, 0x00ED, 0x00AA, 0x0042, 0x0075, 0x0071, 0x00D7, 0x0015, 
	 0x00D7, 0x002E, 0x00AA, 0x0001, 0x0033, 0x0057, 0x0021, 0x004D, 
	 0x0035, 0x0099, 0x0039, 0x0000, 0x0057, 0x002D, 0x000E, 0x0000, 
	 0x0040, 0x0040, 0x00E0, 0x00E0, 0x0005, 0x0000, 0x000E, 0x0000, 
	 0x0064, 0x0004, 0x0010, 0x007C, 0x008A, 0x0044, 0x0082, 0x0000, 
	 0x0024, 0x0000, 0x00C3, 0x0000, 0x0023, 0x00C3, 0x00C4, 0x0000, 
	 0x0023, 0x00C4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x000C, 0x0004, 0x000C, 0x000C, 0x0008, 0x000E, 0x000C, 0x001F, 
	 0x000C, 0x000C, 0x0032, 0x0022, 0x0062, 0x0000, 0x0000, 0x0000, 
	 0x0030, 0x0020, 0x0030, 0x0030, 0x0020, 0x0030, 0x0038, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 9 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x007E, 0x007E, 0x007E, 0x00A8, 0x0008, 0x0000, 
	 0x003C, 0x0018, 0x007E, 0x007E, 0x0070, 0x003E, 0x003C, 0x007E, 
	 0x007E, 0x007E, 0x0000, 0x0000, 0x00FB, 0x0077, 0x00FB, 0x0000, 
	 0x0000, 0x0010, 0x0028, 0x0028, 0x007C, 0x0004, 0x001C, 0x0008, 
	 0x0010, 0x0008, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0024, 0x0018, 0x0042, 0x0042, 0x0030, 0x0002, 0x0004, 0x0042, 
	 0x0042, 0x0042, 0x0000, 0x0000, 0x0020, 0x0000, 0x0004, 0x0042, 
	 0x0044, 0x0024, 0x0044, 0x0042, 0x0044, 0x0044, 0x0044, 0x0042, 
	 0x0042, 0x0010, 0x0020, 0x0022, 0x0002, 0x00C6, 0x0046, 0x0042, 
	 0x0042, 0x0042, 0x0042, 0x0042, 0x0092, 0x0042, 0x0082, 0x0082, 
	 0x0044, 0x0082, 0x0042, 0x0008, 0x0000, 0x0010, 0x0010, 0x0000, 
	 0x0010, 0x0000, 0x0002, 0x0000, 0x0040, 0x0000, 0x0030, 0x0000, 
	 0x0002, 0x0010, 0x0020, 0x0002, 0x0010, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0008, 0x0010, 0x0010, 0x0000, 0x0042, 
	 0x00A1, 0x00AA, 0x008A, 0x0000, 0x0052, 0x00B5, 0x0002, 0x0000, 
	 0x0055, 0x00AB, 0x00B6, 0x0082, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x00B5, 0x002A, 0x002A, 0x0000, 0x0015, 0x0051, 0x0021, 0x008B, 
	 0x002D, 0x0089, 0x0002, 0x0000, 0x0055, 0x002B, 0x000A, 0x0000, 
	 0x0060, 0x0060, 0x00E0, 0x00E0, 0x0006, 0x0000, 0x000C, 0x0000, 
	 0x0024, 0x0004, 0x0010, 0x0038, 0x0053, 0x0066, 0x008B, 0x0000, 
	 0x0024, 0x0000, 0x0066, 0x0000, 0x0026, 0x0066, 0x0064, 0x0000, 
	 0x0026, 0x0064, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 10 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x007E, 0x007E, 0x00B8, 0x003B, 0x0000, 
	 0x0018, 0x0010, 0x003C, 0x003C, 0x0060, 0x003E, 0x0038, 0x007E, 
	 0x003C, 0x003C, 0x0000, 0x0000, 0x0073, 0x0066, 0x0073, 0x0000, 
	 0x0000, 0x0010, 0x0028, 0x0000, 0x0010, 0x0000, 0x0000, 0x0018, 
	 0x0010, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0018, 0x0010, 0x003C, 0x003C, 0x0020, 0x003E, 0x0038, 0x007E, 
	 0x003C, 0x003C, 0x0000, 0x0000, 0x0040, 0x0000, 0x0002, 0x003C, 
	 0x0038, 0x0018, 0x003E, 0x003C, 0x003E, 0x007E, 0x007E, 0x003C, 
	 0x0042, 0x007C, 0x0070, 0x0042, 0x0002, 0x0082, 0x0042, 0x003C, 
	 0x003E, 0x003C, 0x003E, 0x003C, 0x00FE, 0x0042, 0x0082, 0x0082, 
	 0x00C6, 0x0082, 0x007E, 0x0008, 0x0000, 0x0010, 0x0000, 0x0000, 
	 0x0018, 0x0000, 0x0002, 0x0000, 0x0040, 0x0000, 0x0000, 0x0000, 
	 0x0002, 0x0000, 0x0000, 0x0002, 0x0018, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0030, 0x0010, 0x000C, 0x0000, 0x003C, 
	 0x00EF, 0x00AE, 0x000E, 0x0000, 0x0077, 0x0092, 0x000E, 0x0000, 
	 0x0053, 0x0049, 0x00A2, 0x000E, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0092, 0x0024, 0x00EE, 0x0001, 0x0077, 0x0077, 0x0071, 0x0009, 
	 0x00A5, 0x00BB, 0x0003, 0x0000, 0x0057, 0x00E9, 0x000A, 0x0000, 
	 0x0040, 0x0040, 0x00C0, 0x00C0, 0x0004, 0x0000, 0x0008, 0x0000, 
	 0x0020, 0x0004, 0x0010, 0x0010, 0x0022, 0x0044, 0x0072, 0x0000, 
	 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0024, 0x0024, 0x0000, 
	 0x0024, 0x0024, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},

	{	/* Scan line 11 */
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x007E, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0028, 0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 
	 0x0020, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0078, 0x0000, 0x001E, 0x0000, 0x0000, 
	 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0024, 0x0000, 0x0024, 0x0000, 0x0024, 0x0024, 0x0024, 0x0000, 
	 0x0024, 0x0024, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	}
};

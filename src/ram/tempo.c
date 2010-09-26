short	tmpomlt;
short	tmpoval;

main()
{
	register short ti;

	ti  = ( (tmpomlt + 50) * tmpoval) / 100;
	ti  = (short)( (192000L / ti) - 1);

	ti  = (tmpomlt + 50) * tmpoval;
	ti  = (short)( (19200000L / ti) - 1);
}

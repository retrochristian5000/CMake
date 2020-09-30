file(WRITE "${OUTFILE}" [[int main(void)
{
  return 0;
}
]])
file(TOUCH "${STAMPFILE}")
file(WRITE "${DEPFILE}" "${OUTFILE}: ${INFILE}\n")

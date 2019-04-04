/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//
//  File:               TCCFileIO.cc
//  Description:        TCC Useful Functions: FileIO Functions
//  Rev:                <RnXnn>
//  Prodnr:             CNL 113 472
//  Updated:            2009-04-09
//  Contact:            http://ttcn.ericsson.se
//
///////////////////////////////////////////////////////////////////////////////

#include "TCCFileIO_Functions.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <map>
#include <string>
#include <sstream>


#define BUF_SIZE 512

namespace TCCFileIO__Functions
{

/* http://www-h.eng.cam.ac.uk/help/tpl/unix/HP/bsd_to_hpux.html contains more
   possible solutions.  */
#if defined (SOLARIS) || defined (SOLARIS8)
#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

int flock (int fd, int op)
{
  int i;
  switch (op)
    {
      case LOCK_SH:
      case LOCK_EX:
        i = lockf (fd, F_LOCK, 0);
        break;
      case LOCK_SH | LOCK_NB:
      case LOCK_EX | LOCK_NB:
        i = lockf (fd, F_TLOCK, 0);
        if (i == -1)
          if (errno == EAGAIN || errno == EACCES)
            errno = EWOULDBLOCK;
        break;
      case LOCK_UN:
        i = lockf (fd, F_ULOCK, 0);
        break;
      default:
        i = -1;
        errno = EINVAL;
        break;
    }
  return i;
}
#endif

#ifdef ENABLE_CHECKING
/* For storing the opened files globally.  */
static std::map<int, std::string> opened_files;
#endif

#ifdef VERBOSE_DEBUG
void f__FIO__warning (const char *pl__msg, ...)
{
  TTCN_Logger::begin_event (TTCN_WARNING);
  TTCN_Logger::log_event ("TCCFileIO: ");
  va_list ap;
  va_start (ap, pl__msg);
  TTCN_Logger::log_event_va_list (pl__msg, ap);
  va_end (ap);
  TTCN_Logger::end_event ();
}

void f__FIO__error (const char *pl__msg, ...)
{
  TTCN_Logger::begin_event (TTCN_ERROR);
  TTCN_Logger::log_event ("TCCFileIO: ");
  va_list ap;
  va_start (ap, pl__msg);
  TTCN_Logger::log_event_va_list (pl__msg, ap);
  va_end (ap);
  TTCN_Logger::end_event ();
}
#else
void f__FIO__warning (const char *pl__msg, ...)
{
}

void f__FIO__error (const char *pl__msg, ...)
{
}
#endif

/* Function: f__FIO__get__error__string

   Returns the actual error message.

   Returns:
   The actual error message as a string.  */
CHARSTRING f__FIO__get__error__string ()
{
  return CHARSTRING (strerror (errno));
}

/* Function: f__FIO__get__error__code

   Returns the actual error code.

   Returns:
   The actual error message as a string.  */
INTEGER f__FIO__get__error__code ()
{
  return INTEGER (errno);
}

/* Function: f__FIO__realize__error

   Builds the error message.  (For internal use only.)

   Returns:
   Always -1.  */
INTEGER f__FIO__realize__error (const char* pl__func,
  const char* pl__msg, const char* pl__file,
  const int pl__line)
{
  f__FIO__error ("%s:%s:%s(%s:%i)",pl__func,pl__msg,strerror (errno),pl__file,pl__line);
  /* Always return -1 on error.  */
  return INTEGER (-1);
}

#ifdef ENABLE_CHECKING
/* Function: f__FIO__opened

   Checks if the file with the given name is already opened.  This function
   runs in linear time.  (For internal use only.)

   Parameters:
     NAME - The name of the file.

   Returns:
   The file descriptor of the file or -1 if the file is not yet opened.  */
INTEGER f__FIO__opened (const CHARSTRING& pl__name)
{
  /* Linear time.  */
  for (std::map<int, std::string>::const_iterator iter =
    opened_files.begin (); iter != opened_files.end (); iter++)
    {
      if ((iter->second).c_str () == pl__name)
        return INTEGER (iter->first);
    }
  return INTEGER (-1);
}

/* Function: f__FIO__opened

   Checks if there is a file opened with the given file descriptor.  This
   function runs in logarithmic time.  (For internal use only.)

   Parameters:
     FD - The descriptor for the file.

   Returns:
   The name of the file or an empty string if the file is not yet opened.  */
CHARSTRING f__FIO__opened (const INTEGER& pl__fd)
{
  /* Logarithmic time.  */
  std::map<int, std::string>::const_iterator iter =
    opened_files.find (pl__fd);
  return (iter != opened_files.end () ?
    CHARSTRING ((iter->second).c_str ()) : CHARSTRING ());
}
#endif

/* Function: f__FIO__open

   Opens a file with the given name in the given mode.  It creates the file if
   it doesn't exist.  (For internal use only.)

   Parameters:
     NAME - The name of the file.
     FLAGS - The way to open the file.

   Returns:
   The file descriptor for the file or -1 on error.  The error message is
   available through the f__FIO__get__error function.  */
INTEGER f__FIO__open (const CHARSTRING& pl__name, const int pl__flags)
{
#ifdef ENABLE_CHECKING
  if (!(f__FIO__opened (pl__name) < 0))
    return f__FIO__realize__error ("f__FIO__open",
      "The file is already opened", __FILE__, __LINE__);
#endif
  int fd = open (pl__name, pl__flags, S_IRUSR | S_IWUSR | S_IRGRP |
    S_IROTH);
  if (fd < 0)
    return f__FIO__realize__error ("f__FIO__open", "Cannot open file",
      __FILE__, __LINE__);
#ifdef ENABLE_CHECKING
  opened_files[fd] = (const char *) pl__name;
#endif
  return INTEGER (fd);
}

/* Function: f__FIO__open__rdonly

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for reading only.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns.  */
INTEGER f__FIO__open__rdonly (const CHARSTRING& pl__name)
{
  return f__FIO__open (pl__name, O_RDONLY);
}

/* Function: f__FIO__open__append__wronly

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for writing only.  If the file already exists it is opened in appending
   mode.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns.  */
INTEGER f__FIO__open__append__wronly (const CHARSTRING& pl__name)
{
  return f__FIO__open (pl__name, O_APPEND | O_WRONLY | O_CREAT);
}

/* Function: f__FIO__open__append__rdwr

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for reading and writing.  If the file already exists it is opened in
   appending mode.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns.  */
INTEGER f__FIO__open__append__rdwr (const CHARSTRING& pl__name)
{
  return f__FIO__open (pl__name, O_APPEND | O_RDWR | O_CREAT);
}

/* Function: f__FIO__open__trunc__wronly

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for writing only.  If the file was not empty it is truncated.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns.  */
INTEGER f__FIO__open__trunc__wronly (const CHARSTRING& pl__name)
{
  return f__FIO__open (pl__name, O_TRUNC | O_WRONLY | O_CREAT);
}

/* Function: f__FIO__open__trunc__rdwr

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for reading and writing.  If the file was not empty it is truncated.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns.  */
INTEGER f__FIO__open__trunc__rdwr (const CHARSTRING& pl__name)
{
  return f__FIO__open (pl__name, O_TRUNC | O_RDWR | O_CREAT);
}

/* Function: f__FIO__close

   Closes a file associated with the given file descriptor.

   Parameters:
     FD - The file descriptor to close.

   Returns:
   What the POSIX function close returns.  */
INTEGER f__FIO__close (const INTEGER& pl__fd)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__close", "The file is not opened",
      __FILE__, __LINE__);
  opened_files.erase (iter);
#endif
  return close (pl__fd);
}

/* Function: f__FIO__open__append__wronly__excl

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for writing only in exclusive mode.  If the file already exists it is
   opened in appending mode.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns or -1 if the file cannot be locked.  */
INTEGER f__FIO__open__append__wronly__excl (const CHARSTRING& pl__name)
{
  int fd = f__FIO__open (pl__name, O_APPEND | O_WRONLY | O_CREAT);
  if (!(fd < 0))
    if (flock (fd, LOCK_EX | LOCK_NB) < 0)
      {
        f__FIO__close (fd);
        return f__FIO__realize__error ("f__FIO__open__append__wronly__excl",
          "Cannot lock file", __FILE__, __LINE__);
      }
  return fd;
}

/* Function: f__FIO__open__append__rdwr__excl

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for reading and writing in exclusive mode.  If the file already exists it
   is opened in appending mode.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns or -1 if the file cannot be locked.  */
INTEGER f__FIO__open__append__rdwr__excl (const CHARSTRING& pl__name)
{
  int fd = f__FIO__open (pl__name, O_APPEND | O_RDWR | O_CREAT);
  if (!(fd < 0))
    if (flock (fd, LOCK_EX | LOCK_NB) < 0)
      {
        f__FIO__close (fd);
        return f__FIO__realize__error ("f__FIO__open__append__rdwr__excl",
          "Cannot lock file", __FILE__, __LINE__);
      }
  return fd;
}

/* Function: f__FIO__open__trunc__wronly__excl

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for writing only in exclusive mode.  If the file was not empty it is
   truncated.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns or -1 if the file cannot be locked.  */
INTEGER f__FIO__open__trunc__wronly__excl (const CHARSTRING& pl__name)
{
  int fd = f__FIO__open (pl__name, O_TRUNC | O_WRONLY | O_CREAT);
  if (!(fd < 0))
    if (flock (fd, LOCK_EX | LOCK_NB) < 0)
      {
        f__FIO__close (fd);
        return f__FIO__realize__error ("f__FIO__open__trunc__wronly__excl",
          "Cannot lock file", __FILE__, __LINE__);
      }
  return fd;
}

/* Function: f__FIO__open__trunc__rdwr__excl

   A wrapper function for f__FIO__open.  It opens the file with the given name
   for reading and writing in exclusive mode.  If the file was not empty it is
   truncated.

   Parameters:
     NAME - The name of the file.

   Returns:
   What f__FIO__open returns or -1 if the file cannot be locked.  */
INTEGER f__FIO__open__trunc__rdwr__excl (const CHARSTRING& pl__name)
{
  int fd = f__FIO__open (pl__name, O_TRUNC | O_RDWR | O_CREAT);
  if (!(fd < 0))
    if (flock (fd, LOCK_EX | LOCK_NB) < 0)
      {
        f__FIO__close (fd);
        return f__FIO__realize__error ("f__FIO__open__trunc__rdwr__excl",
          "Cannot lock file", __FILE__, __LINE__);
      }
  return fd;
}

/* Function:  f__FIO__seek__home

   Moves the file pointer to the beginning of the file.

   Parameters:
     FD - A file descriptor.

   Returns:
   What the POSIX function lseek returns.  */
INTEGER f__FIO__seek__home (const INTEGER& pl__fd)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__seek__home",
      "The file is not opened", __FILE__, __LINE__);
#endif
  return lseek (pl__fd, 0, SEEK_SET);
}

/* Function: f__FIO__seek__end

   Moves the file pointer to the end of the file.

   Parameters:
     FD - A file descriptor.

   Returns:
   What the POSIX function lseek returns.  */
INTEGER f__FIO__seek__end (const INTEGER& pl__fd)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__seek__end",
      "The file is not opened", __FILE__, __LINE__);
#endif
  return lseek (pl__fd, 0, SEEK_END);
}

/* Function: f__FIO__seek__forward

   Moves the file pointer forward with a given number of bytes from the
   current position.

   Parameters:
     FD - A file descriptor.
     BYTES - The number of bytes.

   Returns:
   What the POSIX function lseek returns.  */
INTEGER f__FIO__seek__forward (const INTEGER& pl__fd,
  const INTEGER& pl__bytes)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__seek__forward",
      "The file is not opened", __FILE__, __LINE__);
#endif
  return lseek (pl__fd, pl__bytes, SEEK_CUR);
}

/* Function: f__FIO__seek__backward

   Moves the file pointer backward with a given number of bytes from the
   current position.

   Parameters:
     FD - A file descriptor.
     BYTES - The number of bytes.

   Returns:
   What the POSIX function lseek returns.  */
INTEGER f__FIO__seek__backward (const INTEGER& pl__fd,
  const INTEGER& pl__bytes)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__seek__backward",
      "The file is not opened", __FILE__, __LINE__);
#endif
  return lseek (pl__fd, -pl__bytes, SEEK_CUR);
}

/* Function: f__FIO__flush

   Transfers ("flushes") all modified in-core data of the file referred to by
   the file descriptor to the disk device.

   Parameters:
     FD - A file descriptor.

   Returns:
   What fsync returns.  */
INTEGER f__FIO__flush (const INTEGER& pl__fd)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__flush", "The file is not opened",
      __FILE__, __LINE__);
#endif
  return fsync ((int) pl__fd);
}

/* Function: f__FIO__write

   Writes the given data to the file associated with the given file
   descriptor.  It works with textual and binary data.  (For internal use
   only.)

   Parameters:
     FD - A file descriptor.
     DATA - The data to write to the file.

   Returns:
   The number of bytes written or -1 on error.  */
template <class T>
INTEGER f__FIO__write (const INTEGER& pl__fd, const T& pl__data,
  bool pl__flush = false)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::const_iterator iter =
    opened_files.find ((int) pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__write", "The file is not opened",
      __FILE__, __LINE__);
#endif
  int bytes = write (pl__fd, (const void *) pl__data, pl__data.lengthof ());
  if (bytes < 0)
    return f__FIO__realize__error ("f__FIO__write", "Cannot write to file",
      __FILE__, __LINE__);
  if (pl__flush)
    f__FIO__flush (pl__fd);
  /* We can safely return here.  */
  return INTEGER (bytes);
}

/* Function: f__FIO__write__text

   A wrapper function for f__FIO__write.  It writes textual data.

   Parameters:
     FD - A file descriptor.
     TEXT - The text to write to the file.

   Returns:
   What f__FIO__write returns.  */
INTEGER f__FIO__write__text (const INTEGER& pl__fd,
  const CHARSTRING& pl__text)
{
  return f__FIO__write<CHARSTRING> (pl__fd, pl__text);
}

/* Function: f__FIO__write__text__flush

   A wrapper function for f__FIO__write.  It writes textual data and calls
   f__FIO__flush.

   Parameters:
     FD - A file descriptor.
     TEXT - The text to write to the file.

   Returns:
   What f__FIO__write returns.  */
INTEGER f__FIO__write__text__flush (const INTEGER& pl__fd,
  const CHARSTRING& pl__text)
{
  return f__FIO__write<CHARSTRING> (pl__fd, pl__text, true);
}

/* Function: f__FIO__write__data

   A wrapper function for f__FIO__write.  It writes binary data.

   Parameters:
     FD - A file descriptor.
     DATA - The data to write to the file.

   Returns:
   What f__FIO__write returns.  */
INTEGER f__FIO__write__data (const INTEGER& pl__fd,
  const OCTETSTRING& pl__data)
{
  return f__FIO__write<OCTETSTRING> (pl__fd, pl__data);
}

/* Function: f__FIO__write__data__flush

   A wrapper function for f__FIO__write.  It writes binary data and calls
   f__FIO__flush.

   Parameters:
     FD - A file descriptor.
     DATA - The data to write to the file.

   Returns:
   What f__FIO__write returns.  */
INTEGER f__FIO__write__data__flush (const INTEGER& pl__fd,
  const OCTETSTRING& pl__data)
{
  return f__FIO__write<OCTETSTRING> (pl__fd, pl__data, true);
}

/* Function: f__FIO__read

   Reads a given number of bytes to a given buffer from a file associated with
   the given file descriptor.  (For internal use only.)

   Parameters:
     FD - A file descriptor.
     DATA - The buffer where the data will be stored.
     BYTES - The number of bytes to read.

   Returns:
   The number of bytes read or -1 if the requested number of bytes couldn't be
   read.  */
template <class T, class U>
INTEGER f__FIO__read (const INTEGER& pl__fd, T& pl__data,
  const INTEGER& pl__bytes)
{
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__read", "The file is not opened",
      __FILE__, __LINE__);
#endif
  int bytes = 0;
  U *buf = (U *) Malloc (sizeof (U) * ((int) pl__bytes));
  memset (buf, 0, sizeof (U) * ((int) pl__bytes));
  bytes = read (pl__fd, buf, pl__bytes);
  if (bytes < 0)
    {
      Free (buf);
      return f__FIO__realize__error ("f__FIO__read", "Read error", __FILE__,
        __LINE__);
    }
  pl__data = T (bytes, buf);
  Free (buf);
  return INTEGER (bytes);
}

/* Function: f__FIO__read__text

   A wrapper function for f__FIO__read.  It reads textual data.

   Parameters:
     FD - A file descriptor.
     TEXT - The buffer for storing the text.
     BYTES - The number of bytes to read.

   Returns:
   What f__FIO__read returns.  */
INTEGER f__FIO__read__text (const INTEGER& pl__fd, CHARSTRING& pl__text,
  const INTEGER& pl__bytes)
{
  return f__FIO__read<CHARSTRING, char> (pl__fd, pl__text, pl__bytes);
}

/* Function: f__FIO__read__data

   A wrapper function for f__FIO__read.  It reads binary data.

   Parameters:
     FD - A file descriptor.
     DATA - The buffer for storing the data.
     BYTES - The number of bytes to read.

   Returns:
   What f__FIO__read returns.  */
INTEGER f__FIO__read__data (const INTEGER& pl__fd, OCTETSTRING& pl__data,
  const INTEGER& pl__bytes)
{
  return f__FIO__read<OCTETSTRING, unsigned char> (pl__fd, pl__data,
    pl__bytes);
}

/* Function: f__FIO__read__text__until

   It reads textual data until a given pattern is matched.

   Parameters:
     FD - A file descriptor.
     TEXT - The buffer for storing the text.
     SEPARATOR - The separator pattern.

   Returns:
   The number of bytes read from the file, not including the delimiter.  In
   case of error it returns -1.  */
INTEGER f__FIO__read__text__until (const INTEGER& pl__fd,
  CHARSTRING& pl__text, const CHARSTRING& pl__separator)
{
  char *buf = NULL; /* For the whole line.  */
  char *out = NULL;
  char buf_tmp[BUF_SIZE + 1]; /* For reading chunks.  */
  unsigned int buf_size = BUF_SIZE << 1;
  unsigned int byte_num = 0;
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__read__text__until",
      "The file is not opened", __FILE__, __LINE__);
#endif
  while (true)
    {
      bzero (buf_tmp, BUF_SIZE);
      /* Try to read BUF_SIZE bytes.  */
      int bytes = read (pl__fd, buf_tmp, BUF_SIZE);
      buf_tmp[bytes] = '\0'; /* Never forget.  */
      if (bytes < 0)
        {
          if (buf)
            Free (buf);
          return f__FIO__realize__error ("f__FIO__read__text__until",
            "Read error", __FILE__, __LINE__);
        }
      else if (bytes == 0)
        {
          if (buf != NULL)
            Free (buf);
          return f__FIO__realize__error ("f__FIO__read__text__until",
            "End of file", __FILE__, __LINE__);
        }
      if (buf == NULL || byte_num + bytes >= buf_size)
	{
	  int new_size = 2 * buf_size;
	  char *new_buf = (char *) Realloc (buf, sizeof (char) * new_size);
	  /* Blank it first time.  */
	  if (new_size == BUF_SIZE << 2)
	    bzero (new_buf, new_size);
	  if (!new_buf)
	    {
              if (buf != NULL)
                Free (buf);
              return f__FIO__realize__error ("f__FIO__read__text__until",
                "Cannot allocate memory", __FILE__, __LINE__);
 	    }
	  buf = new_buf;
	  buf_size = new_size;
	}
      memcpy (buf + byte_num, buf_tmp, bytes);
      byte_num += bytes;
      /* We always have space for the trailing '\0'.  */
      buf[byte_num] = '\0';
      if (strlen (buf) >= strlen (pl__separator))
	if (strstr (buf, pl__separator) != NULL)
	  break;
    }
  int back = buf + byte_num - strstr (buf, pl__separator) -
    strlen (pl__separator);
  /* Ignore the characters after the delimiter.  */
  lseek (pl__fd, -back, SEEK_CUR);
  if (out == NULL)
    /* I miss "Calloc" a lot.  */
    out = (char *) Malloc (((strstr (buf, pl__separator) - buf) + 1) *
      sizeof (char));
  int out_len = strstr (buf, pl__separator) - buf;
  bzero (out, out_len + 1);
  memcpy (out, buf, out_len);
  out[out_len] = '\0';
  pl__text = CHARSTRING (out_len, out);
  Free (out);
  Free (buf);
  return out_len;
}

/* Function: f__FIO__read__data__until

   It reads binary data until a given pattern is matched.

   Parameters:
     FD - A file descriptor.
     TEXT - The buffer for storing the data.
     SEPARATOR - The separator pattern.

   Returns:
   The number of bytes read from the file, not including the delimiter.  In
   case of error it returns -1.  */
INTEGER f__FIO__read__data__until (const INTEGER& pl__fd,
  OCTETSTRING& pl__data, const OCTETSTRING& pl__separator)
{
  int buf_size = BUF_SIZE;
  int buf_num = 0;
  unsigned char *buf = NULL;
  unsigned char c;
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__read__data__until",
      "The file is not opened", __FILE__, __LINE__);
#endif
  while (true)
    {
      int bytes = read (pl__fd, &c, 1);
      if (bytes < 0)
        {
          if (buf)
            Free (buf);
          /* Return -1 and print out a probably useful error message.  */
          return f__FIO__realize__error ("f__FIO__read__data__until",
            "Read error", __FILE__, __LINE__);
        }
      /* EOF is reached before we first matched the pattern.  */
      else if (bytes == 0)
        {
          if (buf)
            Free (buf);
          return f__FIO__realize__error ("f__FIO__read__data__until",
            "End of file", __FILE__, __LINE__);
        }
      if (buf == NULL || buf_num == (int) (sizeof (unsigned char) * buf_size))
        {
          int new_size = buf_size * 2;
          unsigned char *new_buf = (unsigned char *) Realloc (buf,
            sizeof (unsigned char) * new_size);
          if (!new_buf)
            {
              if (buf)
                Free (buf);
              return f__FIO__realize__error ("f__FIO__read__data__until",
                "Cannot allocate memory", __FILE__, __LINE__);
            }
          buf = new_buf;
          buf_size = new_size;
        }
      buf[buf_num++] = c;
      if (buf_num >= pl__separator.lengthof () &&
          !memcmp ((const unsigned char *) (buf + buf_num -
            pl__separator.lengthof ()), (const unsigned char *) pl__separator,
            pl__separator.lengthof ()))
        break;
    }
  pl__data = OCTETSTRING (buf_num - pl__separator.lengthof (), buf);
  Free (buf);
  return INTEGER (pl__data.lengthof ());
}

/*Function:  f__FIO__set__filedescriptor__previousline
   It sets the file descriptor to the begin of the previous line.

   Parameters:
     FD - A file descriptor.

   Returns:
    If there was no problem, then the return value will be 1.  */
INTEGER f__FIO__set__filedescriptor__previousline (const INTEGER& pl__fd)
{
  char buf_tmp[BUF_SIZE+ 1]; /* For reading chunks.  */
  int bytes = 0;
  int founded = 0;

#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__read__text__until",
      "The file is not opened", __FILE__, __LINE__);
#endif
  while (true) {    
      bzero (buf_tmp, BUF_SIZE);
      /* Try to read BUF_SIZE bytes.  */
      int length = lseek(pl__fd,0,SEEK_CUR);
      if (length >= BUF_SIZE){
          lseek(pl__fd,-BUF_SIZE,SEEK_CUR);
          bytes = read (pl__fd, buf_tmp, BUF_SIZE);
          length = BUF_SIZE;
          buf_tmp[bytes] = '\0'; /* Never forget.  */
        }
      else if (length>0){
          lseek(pl__fd,-length,SEEK_CUR);
          bytes = read (pl__fd, buf_tmp, length);
          buf_tmp[bytes] = '\0'; /* Never forget.  */
      }
      else if (length == 0) {
          return 0;
      }
      else {
          return -1;
      }
           
      int p = bytes-1;
      while ((buf_tmp[p] != '\n') && (p != 0) ) {
        p--;
      }
      
      if ((p != 0) && (founded == 0)) {
    	  founded++;
    	  p--;
    	  while ((buf_tmp[p] != '\n') && (p != 0) ) {
    	          p--;
    	  }
    	  if ((p != 0) && (founded == 1)) {
    	      	  lseek(pl__fd,-(length-p)+1,SEEK_CUR);
    	      	  return 1;
    	  }  
      }
           
      if ((p != 0) && (founded == 1)) {
    	  lseek(pl__fd,-(length-p)+1,SEEK_CUR);
    	  return 1; 	      	  
      }
          
      lseek(pl__fd,-length,SEEK_CUR);
  }   
}

/* Function: f__FIO__read__data__TLV

   It reads binary data until a full ASN.1 TLV structure is read.

   Parameters:
     FD - A file descriptor.
     TEXT - The buffer for storing the data.

   Returns:
   The number of bytes read from the file.  In
   case of error it returns -1.  */
INTEGER f__FIO__read__data__TLV (const INTEGER& pl__fd,
  OCTETSTRING& pl__data)
{
  unsigned char *buf = NULL; /* For the whole line.  */
  char buf_tmp[BUF_SIZE]; /* For reading chunks.  */
  unsigned int buf_size = BUF_SIZE << 1;
  unsigned int byte_num = 0;
  ASN_BER_TLV_t tlv;
  size_t tlvlength;
#ifdef ENABLE_CHECKING
  std::map<int, std::string>::iterator iter = opened_files.find (pl__fd);
  if (iter == opened_files.end ())
    return f__FIO__realize__error ("f__FIO__read__data__TLV",
      "The file is not opened", __FILE__, __LINE__);
#endif
  while (true)
    {
      /* Try to read BUF_SIZE bytes.  */
      int bytes = read (pl__fd, buf_tmp, BUF_SIZE);
      if (bytes < 0)
        {
          if (buf)
            Free (buf);
          return f__FIO__realize__error ("f__FIO__read__data__TLV",
            "Read error", __FILE__, __LINE__);
        }
      else if (bytes == 0)
        {
          if (buf != NULL)
            Free (buf);
          return f__FIO__realize__error ("f__FIO__read__data__TLV",
            "End of file", __FILE__, __LINE__);
        }
      if (buf == NULL || byte_num + bytes >= buf_size)
	{
	  int new_size = 2 * buf_size;
	  unsigned char *new_buf = (unsigned char *) Realloc (buf, sizeof (char) * new_size);
	  /* Blank it first time.  */
	  if (new_size == BUF_SIZE << 2)
	    bzero (new_buf, new_size);
	  if (!new_buf)
	    {
              if (buf != NULL)
                Free (buf);
              return f__FIO__realize__error ("f__FIO__read__data__TLV",
                "Cannot allocate memory", __FILE__, __LINE__);
 	    }
	  buf = new_buf;
	  buf_size = new_size;
	}
      memcpy (buf + byte_num, buf_tmp, bytes);
      byte_num += bytes;

      /* Check for TLV structure */
      if (ASN_BER_str2TLV(byte_num,buf,tlv,BER_ACCEPT_ALL))
        {
	  tlvlength = tlv.get_len();
	  break;
        }
    }
  /* Ignore the characters after the delimiter.  */
  lseek (pl__fd, -(byte_num - tlvlength), SEEK_CUR);

  pl__data = OCTETSTRING (tlvlength, buf);
  Free (buf);
  return tlvlength;
}

/* Function: f__FIO__chdir

   It changes the current directory.

   Parameters:
     NAME - The name of the new directory.

   Returns:
   Boolean value for successful or unsuccessful directory change  */
BOOLEAN f__FIO__chdir(const CHARSTRING& pl__name)
{
  if(chdir((const char *)(pl__name)))
  {
    f__FIO__realize__error ("f__FIO__chdir",
                "Cannot change directory", __FILE__, __LINE__);
    return FALSE;
  }
  else
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_FIO_mkdir
// 
//  Purpose:
//    Create a new directory
//
//  Parameters:
//    p_dir_name - *in* *charstring* - name of the directory to create
// 
//  Return Value:
//    boolean - indicate the successful or unsuccessful directory creation
//
//  Errors:
//    In the case of unsuccessful operation the cause of the error can be 
//    queried  by the f_FIO_get_error_code, f_FIO_get_error_string functions
// 
//  Detailed description:
//    The path to the directory must exist, so the existence of the path
//    should be checked, and the missing directories should be created
//    recursively.
// 
///////////////////////////////////////////////////////////////////////////////
BOOLEAN f__FIO__mkdir(const CHARSTRING& pl__dir__name)
{
  if(mkdir((const char *)(pl__dir__name),0777))
  {
    f__FIO__realize__error ("f_FIO_mkdir",
                "Cannot make a directory", __FILE__, __LINE__);
    return FALSE;
  }
  else
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_FIO_rmdir
// 
//  Purpose:
//    Removes a directory
//
//  Parameters:
//    p_dir_name - *in* *charstring* - name of the directory to remove
// 
//  Return Value:
//    boolean - indicate the successful or unsuccessful directory creation
//
//  Errors:
//    In the case of unsuccessful operation the cause of the error can be 
//    queried  by the f_FIO_get_error_code, f_FIO_get_error_string functions
// 
//  Detailed description:
//    The directories must be empty, so the child directories should be 
//    cleaned and deleted recursively.
// 
///////////////////////////////////////////////////////////////////////////////
BOOLEAN f__FIO__rmdir(const CHARSTRING& pl__dir__name)
{
  if(rmdir((const char *)(pl__dir__name)))
  {
    f__FIO__realize__error ("f__FIO__rmdir",
                "Cannot make a directory", __FILE__, __LINE__);
    return FALSE;
  }
  else
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_FIO_fileOrDirExists
// 
//  Purpose:
//    Checks the existence of files and directories.
//
//  Parameters:
//    p_name - *in* *charstring* - name of the file or directory to check
// 
//  Return Value:
//    boolean - indicate the exictense of the file or diectory
//
//  Errors:
//    -
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////
BOOLEAN f__FIO__fileOrDirExists(const CHARSTRING& filename ) {
  struct stat buffer ;
  if(stat( (const char *)filename, &buffer )==0) {
    return TRUE ;
  }
  return FALSE ;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_FIO_stat
// 
//  Purpose:
//    Query the permissions of the file or directory.
//
//  Parameters:
//    p_name - *in* *charstring* - name of the file or directory to check
//    p_permissions - *out* *FIO_permissions* - the permissions of the object
// 
//  Return Value:
//    boolean - indicate sucessfull execution
//
//  Errors:
//    -
// 
//  Detailed description:
//    -
// 
///////////////////////////////////////////////////////////////////////////////

BOOLEAN f__FIO__stat(const CHARSTRING& p__name,
                     FIO__permissions& p__permissions ) {
  struct stat buffer;
  mode_t   st_mode;
  if(stat( (const char *)p__name, &buffer )) {
    f__FIO__realize__error ("f__FIO__stat",
                "Cannot stat", __FILE__, __LINE__);
    return FALSE;
  }
  st_mode=buffer.st_mode;
  p__permissions.set__uid()=st_mode & S_ISUID;
  p__permissions.set__gid()=st_mode & S_ISGID;
  p__permissions.sticky__bit()=st_mode & S_ISVTX;
  p__permissions.owner__read()=st_mode & S_IRUSR;
  p__permissions.owner__write()=st_mode & S_IWUSR;
  p__permissions.owner__execute()=st_mode & S_IXUSR;
  p__permissions.group__read()=st_mode & S_IRGRP;
  p__permissions.group__write()=st_mode & S_IWGRP;
  p__permissions.group__execute()=st_mode & S_IXGRP;
  p__permissions.other__read()=st_mode & S_IROTH;
  p__permissions.other__write()=st_mode & S_IWOTH;
  p__permissions.other__execute()=st_mode & S_IXOTH;
  
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  Function: f_FIO_chmod
// 
//  Purpose:
//    Change the permissions of the file or directory.
//
//  Parameters:
//    p_name - *in* *charstring* - name of the file or directory to check
//    p_permissions - *in* *FIO_permissions* - the permissions of the object
// 
//  Return Value:
//    boolean - indicate sucessfull execution
//
//  Errors:
//    -
// 
//  Detailed description:
//    Change the permissions of the file or directory according to the 
//    p_permissions. If the value of the field is:
//       - true: set the permission
//       - false: clear the permission
//       - omit: doesn't change the permission
// 
///////////////////////////////////////////////////////////////////////////////
BOOLEAN f__FIO__chmod(const CHARSTRING& p__name,
                      const FIO__permissions& p__permissions ) {
  struct stat buffer;
  mode_t   st_mode;
  if(stat( (const char *)p__name, &buffer )) {
    f__FIO__realize__error ("f__FIO__chmod",
                "Cannot stat", __FILE__, __LINE__);
    return FALSE;
  }
  st_mode=buffer.st_mode;
  if(p__permissions.set__uid().ispresent()){
    if(p__permissions.set__uid()()){
      st_mode |= S_ISUID;
    } else {
      st_mode &= ~S_ISUID;
    }
  }
  if(p__permissions.set__gid().ispresent()){
    if(p__permissions.set__gid()()){
      st_mode |= S_ISGID;
    } else {
      st_mode &= ~S_ISGID;
    }
  }
  if(p__permissions.sticky__bit().ispresent()){
    if(p__permissions.sticky__bit()()){
      st_mode |= S_ISVTX;
    } else {
      st_mode &= ~S_ISVTX;
    }
  }
  if(p__permissions.owner__read().ispresent()){
    if(p__permissions.owner__read()()){
      st_mode |= S_IRUSR;
    } else {
      st_mode &= ~S_IRUSR;
    }
  }
  if(p__permissions.owner__write().ispresent()){
    if(p__permissions.owner__write()()){
      st_mode |= S_IWUSR;
    } else {
      st_mode &= ~S_IWUSR;
    }
  }
  if(p__permissions.owner__execute().ispresent()){
    if(p__permissions.owner__execute()()){
      st_mode |= S_IXUSR;
    } else {
      st_mode &= ~S_IXUSR;
    }
  }
  if(p__permissions.group__read().ispresent()){
    if(p__permissions.group__read()()){
      st_mode |= S_IRGRP;
    } else {
      st_mode &= ~S_IRGRP;
    }
  }
  if(p__permissions.group__write().ispresent()){
    if(p__permissions.group__write()()){
      st_mode |= S_IWGRP;
    } else {
      st_mode &= ~S_IWGRP;
    }
  }
  if(p__permissions.group__execute().ispresent()){
    if(p__permissions.group__execute()()){
      st_mode |= S_IXGRP;
    } else {
      st_mode &= ~S_IXGRP;
    }
  }
  if(p__permissions.other__read().ispresent()){
    if(p__permissions.other__read()()){
      st_mode |= S_IROTH;
    } else {
      st_mode &= ~S_IROTH;
    }
  }
  if(p__permissions.other__write().ispresent()){
    if(p__permissions.other__write()()){
      st_mode |= S_IWOTH;
    } else {
      st_mode &= ~S_IWOTH;
    }
  }
  if(p__permissions.other__execute().ispresent()){
    if(p__permissions.other__execute()()){
      st_mode |= S_IXOTH;
    } else {
      st_mode &= ~S_IXOTH;
    }
  }
  
  if(chmod( (const char *)p__name, st_mode )) {
    f__FIO__realize__error ("f__FIO__chmod",
                "Cannot chmod", __FILE__, __LINE__);
    return FALSE;
  }

  return TRUE;
}

}

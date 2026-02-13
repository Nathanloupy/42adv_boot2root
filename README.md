# boot2root

This project is a penetration testing challenge on the "BornToSecHackMe" vulnerable virtual machine. The goal is to gain root access starting from no credentials, documenting the full exploitation chain from reconnaissance to privilege escalation.

### Project Structure

- `writeup1.md` : main exploitation path covering reconnaissance, web exploitation, credential discovery, user progression, and privilege escalation
- `writeup2.md` and `bonus/` : bonus writeups demonstrating alternative privilege escalation methods using the Dirty COW vulnerability
- `scripts/` : helper scripts for various challenges

**Web Application Security:**
- Directory enumeration
- Credential leakage in logs
- SQL injection (MySQL INTO OUTFILE)
- Web shell deployment
- Reverse shell establishment

**Binary Exploitation:**
- Buffer overflow vulnerabilities
- ret2libc attacks
- SUID binary exploitation
- Binary reverse engineering

**Linux Privilege Escalation:**
- Dirty COW kernel vulnerability (CVE-2016-5195)
- SUID binary abuse
- Crontab manipulation
- Sudoers modification
- SSH key persistence

**Programming/Scripting:**
- Python automation (file parsing, turtle graphics)
- C exploit development
- Shellcode generation
- Payload crafting

## Grading

This project is part of my advanced curriculum at 42 School.

- **Date of completion:** 2025-10-21
- **Grade:** 125/100

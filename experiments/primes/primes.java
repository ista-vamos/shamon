import java.util.*;
import java.util.Random;
import java.lang.Math;
import java.util.Scanner;
/* measure time */
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;

public class primes {
	public static void main(String[] args) {
		long current=1;
		int count=1;
		int tcount = 10;
		boolean interactive=false;
		boolean erastothenes=false;
		if(args.length>0)
		{
			if(args[0].equals("-i"))
			{
				interactive=true;
			}
			else
			{
				tcount = Integer.parseInt(args[0]);
				if(args.length>1)
				{
					if(args[1].equals("-e"))
					{
						erastothenes=true;
					}
				}
			}
		}
		else
		{
			System.out.println("Usage: primes [-i | n [-e]]");
			return;
		}
		if(interactive)
		{
			Scanner in = new Scanner(System.in);
			try
			{
				while(true)
				{
					String line=in.nextLine();
					if(line.equals("quit"))
					{
						System.out.println("byequit");
						break;
					}
					else if(line.startsWith("isprime "))
					{
						long number=Long.parseLong(line.substring(8));
						System.out.println(number+" is"+(bailliePSW(number)?"":" not")+" prime");
					}
					else
					{
						long index=Long.parseLong(line);
						if(index<=1)
						{
							System.out.println("#1: 2");
						}
						else
						{
							count=1;
							current=1;
							while(count<index)
							{
								current+=2;
								if(bailliePSW(current))
								{
									count++;
								}
							}
							System.out.println("#" + count + ": "+current);
						}
					}
				}
			}
			catch(NoSuchElementException e)
			{
				System.out.println("byeeof");
			}
		}
		else
		{
                        ThreadMXBean mxBean = ManagementFactory.getThreadMXBean();
                        long start = mxBean.getCurrentThreadCpuTime();
			if(tcount>0)
			{
				System.out.println("#1: 2");
			}
			if(erastothenes)
			{
				List<Long> ps = new ArrayList<Long>();
				ps.add(2L);
				while(count<tcount)
				{
					current+=2;
					boolean found=false;
					for(Long i : ps)
					{
						if(current % i==0)
						{
							found=true;
							break;
						}
					}
					if(!found)
					{
						ps.add(current);
						count++;
						System.out.println("#" + count + ": "+current);
					}
				}
			}
			else
			{
				while(count<tcount)
				{
					current+=2;
					if(bailliePSW(current))
					{
						count++;
						System.out.println("#" + count + ": "+current);
					}
				}
			}
                        long end = mxBean.getCurrentThreadCpuTime();
                        System.err.println("time: " + ((end - start) / 1000000000.0) + " seconds.");
		}
		
	}

	private static boolean bailliePSW(long n)
	{
		if(((n%2)==0)||((n%3)==0)||((n%5)==0)||n<3)
		{
			return n==2||n==3||n==5;
		}
		if(!millerRabin(n))
		{
			// if(n==29741)
			// {
			// 	System.out.println("MR");
			// }
			return false;
		}
		long D=5;
		long count=0;
		while(jacobi(n,D)!=-1)
		{
			D=-(D+(D>=0?2:-2));
			count++;
			if(count==20&&perfectSquare(n))
			{
				// if(n==29741)
				// {
				// 	System.out.println("PS");
				// }
				return false;
			}
		}
		// if(n==29741)
		// {
		// 	System.out.println("D: "+D);
		// }
		return lucasPPT(D, n);
	}

	private static boolean perfectSquare(long n)
	{
		long lo=2;
		long hi=n/2;
		while(lo<=hi)
		{
			long root=(lo+hi)/2;
			long square=root*root;
			if(square==n)
			{
				return true;
			}
			if(square>n)
			{
				hi=root-1;
			}
			else
			{
				lo=root+1;
			}
		}
		return false;
	}

	private static long longmod(long a, long b)
	{
		return (a < 0) ? (b-1 - (-a-1) % b) : (a % b);
	}

	private static boolean lucasPPT(long D, long n)
	{
		long P=1;
		long Q=(1-D)/4;
		long d=n+1;
		long s=0;
		while(d%2==0)
		{
			s++;
			d/=2;
		}
		String bin = Long.toBinaryString(d);
		// System.out.println(bin+" | " + (n+1) + " = "+d+"*2^"+s + "| Q:" + Q + "| D: "+D);
		long U=1;
		long V=1;
		// long Qmul=Q;
		// long k=1;
		for(char c : bin.substring(1).toCharArray())
		{
			// System.out.println("U_"+k+" = "+U+"| V_"+k+" = "+V);
			long Unext = U*V;
			long Vnext = ((V*V) + (D * U * U));
			// long vnt = (V * V) - 2 * Qmul;
			if(Vnext%2!=0)
			{
				Vnext+=n;
			}
			Vnext/=2;
			// if(longmod(vnt,n)!=longmod(Vnext,n))
			// {
			// 	System.out.println("V error: "+Vnext+"|"+vnt +"|"+Qmul+"|"+k);
			// 	throw new RuntimeException();
			// }
			U=longmod(Unext,n);
			V=longmod(Vnext,n);
			// Qmul = longmod(Qmul * Qmul,n);
			// k=k*2;
			if(c=='1')
			{
				// System.out.println("U_"+k+" = "+U+"/ V_"+k+" = "+V);
				Unext=((P * U) + V);
				if(Unext%2!=0)
				{
					Unext+=n;
				}
				Unext/=2;
				Vnext=((D * U) + (P * V));
				if(Vnext%2!=0)
				{
					Vnext+=n;
				}
				Vnext/=2;
				U=longmod(Unext,n);
				V=longmod(Vnext,n);
				// Qmul=longmod(Qmul * Q,n);
				// k+=1;
			}
			// System.out.println(U+"^"+V);
		}
		if(U==0)
		{
			return true;
		}
		for(long i=0;i<s;i++)
		{
			// System.out.println("U_"+k+" = "+U+"\\ V_"+k+" = "+V);
			if(V==0)
			{
				return true;
			}
			long Unext = U*V;
			long Vnext = ((V*V) + (D * U * U));
			// long vnt = (V * V) - 2 * Qmul;
			if(Vnext%2!=0)
			{
				Vnext+=n;
			}
			Vnext/=2;
			// if(longmod(vnt,n)!=longmod(Vnext,n))
			// {
			// 	System.out.println("V error: "+Vnext+"|"+vnt);
			// 	throw new RuntimeException();
			// }
			U=longmod(Unext,n);
			V=longmod(Vnext,n);
			// Qmul = longmod(Qmul * Qmul,n);
			// k=k*2;
		}
		// System.out.println("U_"+k+" = "+U+"# V_"+k+" = "+V);
		return false;
	}

	private static boolean millerRabin(long n)
	{
		long d = n - 1;
		long r=0;
		while(d % 2==0)
		{
			d=d/2;
			r++;
		}
		long x=1;
		for(long j=0;j<d;j++)
		{
			x = (x * 2) % n;
		}
		if((x==1)||(x == n - 1))
		{
			return true;
		}
		for(long j=0;j<r-1;j++)
		{
			x = (x * x) % n;
			if(x==n-1)
			{
				return true;
			}
		}
		return false;
	}

	private static int jacobi(long n, long a)
	{
		n = longmod(n, a);
		int t = 1;
		while( n != 0 )
		{
			while (n%2==0)
			{
				n/=2;
				long r = longmod(a, 8);
				if(r==3||r==5)
				{
					t = -t;
				}
			}
			long swap=n;
			n=a;
			a=swap;
			if(longmod(n,4)==3 && longmod(a,4)==3)
			{
				t = -t;
			}
			n = longmod(n, a);
		}
		if(a==1)
		{
			return t;
		}
		return 0;
	}
}

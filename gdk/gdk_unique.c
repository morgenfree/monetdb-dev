/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2008-2015 MonetDB B.V.
 */

#include "monetdb_config.h"
#include "gdk.h"
#include "gdk_private.h"
#include "gdk_calc_private.h"

#define VALUE(x)	(vars ? vars + VarHeapVal(vals, (x), width) : vals + (x) * width)
/* BATsubunique returns a bat that indicates the unique tail values of
 * the input bat.  This is essentially the same output as the
 * "extents" output of BATgroup.  The difference is that BATsubunique
 * can optionally take a candidate list, something that doesn't make
 * sense for BATgroup, and does not return the grouping bat.
 *
 * The inputs must be dense-headed, the first input is the bat from
 * which unique rows are selected, the second input is a list of
 * candidates.
 */
BAT *
BATsubunique(BAT *b, BAT *s)
{
	BAT *bn;
	BUN start, end, cnt;
	const oid *cand = NULL, *candend = NULL;
	const void *v;
	const char *vals;
	const char *vars;
	int width;
	oid i, o;
	unsigned short *seen = NULL;
	const char *nme;
	char *ext = NULL;
	Heap *hp = NULL;
	Hash *hs = NULL;
	BUN hb;
	BATiter bi;
	int (*cmp)(const void *, const void *);
#ifndef DISABLE_PARENT_HASH
	bat parent;
#endif

	BATcheck(b, "BATsubunique", NULL);
	if (b->tkey || BATcount(b) <= 1 || BATtdense(b)) {
		/* trivial: already unique */
		if (s) {
			/* we can return a slice of the candidate list */
			oid lo = b->hseqbase;
			oid hi = lo + BATcount(b);
			ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): trivial case: already unique, slice candidates\n",
					  BATgetId(b), BATcount(b),
					  BATgetId(s), BATcount(s));
			b = BATsubselect(s, NULL, &lo, &hi, 1, 0, 0);
			if (b == NULL)
				return NULL;
			bn = BATproject(b, s);
			BBPunfix(b->batCacheid);
			return virtualize(bn);
		}
		/* we can return all values */
		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=NULL): trivial case: already unique, return all\n",
				  BATgetId(b), BATcount(b));
		bn = BATnew(TYPE_void, TYPE_void, BATcount(b), TRANSIENT);
		if (bn == NULL)
			return NULL;
		BATsetcount(bn, BATcount(b));
		BATseqbase(bn, 0);
		BATseqbase(BATmirror(bn), b->hseqbase);
		return bn;
	}

	CANDINIT(b, s, start, end, cnt, cand, candend);

	if (start == end) {
		/* trivial: empty result */
		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): trivial case: empty\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		bn = BATnew(TYPE_void, TYPE_void, 0, TRANSIENT);
		if (bn == NULL)
			return NULL;
		BATsetcount(bn, 0);
		BATseqbase(bn, 0);
		BATseqbase(BATmirror(bn), b->hseqbase);
		return bn;
	}

	if ((b->tsorted && b->trevsorted) ||
	    (b->ttype == TYPE_void && b->tseqbase == oid_nil)) {
		/* trivial: all values are the same */
		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): trivial case: all equal\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		bn = BATnew(TYPE_void, TYPE_void, 1, TRANSIENT);
		if (bn == NULL)
			return NULL;
		BATsetcount(bn, 1);
		BATseqbase(bn, 0);
		BATseqbase(BATmirror(bn), cand ? *cand : b->hseqbase);
		return bn;
	}

	if (cand && BATcount(b) > 16 * BATcount(s)) {
		BAT *nb, *r, *nr;

		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): recurse: few candidates\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		nb = BATproject(s, b);
		if (nb == NULL)
			return NULL;
		r = BATsubunique(nb, NULL);
		if (r == NULL) {
			BBPunfix(nb->batCacheid);
			return NULL;
		}
		nr = BATproject(r, s);
		BBPunfix(nb->batCacheid);
		BBPunfix(r->batCacheid);
		return virtualize(nr);
	}

	assert(b->ttype != TYPE_void);

	bn = BATnew(TYPE_void, TYPE_oid, 1024, TRANSIENT);
	if (bn == NULL)
		return NULL;
	BATseqbase(bn, 0);
	vals = Tloc(b, BUNfirst(b));
	if (b->tvarsized && b->ttype)
		vars = b->T->vheap->base;
	else
		vars = NULL;
	width = Tsize(b);
	cmp = ATOMcompare(b->ttype);
	bi = bat_iterator(b);

	if (b->tsorted || b->trevsorted) {
		const void *prev = NULL;

		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): (reverse) sorted\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		for (;;) {
			if (cand) {
				if (cand == candend)
					break;
				i = *cand++ - b->hseqbase;
				if (i >= end)
					break;
			} else {
				i = start++;
				if (i == end)
					break;
			}
			v = VALUE(i);
			if (prev == NULL || (*cmp)(v, prev) != 0) {
				o = i + b->hseqbase;
				bunfastapp(bn, &o);
			}
			prev = v;
		}
	} else if (ATOMbasetype(b->ttype) == TYPE_bte) {
		unsigned char val;

		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): byte sized atoms\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		assert(vars == NULL);
		seen = GDKzalloc(256 / 16);
		if (seen == NULL)
			goto bunins_failed;
		for (;;) {
			if (cand) {
				if (cand == candend)
					break;
				i = *cand++ - b->hseqbase;
				if (i >= end)
					break;
			} else {
				i = start++;
				if (i == end)
					break;
			}
			val = ((const unsigned char *) vals)[i];
			if (!(seen[val >> 4] & (1 << (val & 0xF)))) {
				seen[val >> 4] |= 1 << (val & 0xF);
				o = i + b->hseqbase;
				bunfastapp(bn, &o);
				if (bn->batCount == 256) {
					/* there cannot be more than
					 * 256 distinct values */
					break;
				}
			}
		}
		GDKfree(seen);
		seen = NULL;
	} else if (ATOMbasetype(b->ttype) == TYPE_sht) {
		unsigned short val;

		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): short sized atoms\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		assert(vars == NULL);
		seen = GDKzalloc(65536 / 16);
		if (seen == NULL)
			goto bunins_failed;
		for (;;) {
			if (cand) {
				if (cand == candend)
					break;
				i = *cand++ - b->hseqbase;
				if (i >= end)
					break;
			} else {
				i = start++;
				if (i == end)
					break;
			}
			val = ((const unsigned short *) vals)[i];
			if (!(seen[val >> 4] & (1 << (val & 0xF)))) {
				seen[val >> 4] |= 1 << (val & 0xF);
				o = i + b->hseqbase;
				bunfastapp(bn, &o);
				if (bn->batCount == 65536) {
					/* there cannot be more than
					 * 65536 distinct values */
					break;
				}
			}
		}
		GDKfree(seen);
		seen = NULL;
	} else if (BATcheckhash(b) ||
		   (b->batPersistence == PERSISTENT &&
		    BAThash(b, 0) == GDK_SUCCEED)
#ifndef DISABLE_PARENT_HASH
		   || ((parent = VIEWtparent(b)) != 0 &&
		       BATcheckhash(BBPdescriptor(-parent)))
#endif
		) {
		BUN lo;
		oid seq;

		/* we already have a hash table on b, or b is
		 * persistent and we could create a hash table, or b
		 * is a view on a bat that already has a hash table */
		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): use existing hash\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		seq = b->hseqbase;
#ifndef DISABLE_PARENT_HASH
		if (b->T->hash == NULL && (parent = VIEWtparent(b)) != 0) {
			BAT *b2 = BBPdescriptor(-parent);
			lo = (BUN) ((b->T->heap.base - b2->T->heap.base) >> b->T->shift) + BUNfirst(b);
			b = b2;
			bi = bat_iterator(b);
		} else
#endif
		{
			lo = BUNfirst(b);
		}
		hs = b->T->hash;
		for (;;) {
			if (cand) {
				if (cand == candend)
					break;
				i = *cand++ - seq;
				if (i >= end)
					break;
			} else {
				i = start++;
				if (i == end)
					break;
			}
			v = VALUE(i);
			for (hb = HASHgetlink(hs, i + lo);
			     hb != HASHnil(hs) && hb >= lo;
			     hb = HASHgetlink(hs, hb)) {
				assert(hb < i + lo);
				if (cmp(v, BUNtail(bi, hb)) == 0) {
					o = hb - lo + seq;
					if (cand == NULL ||
					    SORTfnd(s, &o) != BUN_NONE) {
						/* we've seen this
						 * value before */
						break;
					}
				}
			}
			if (hb == HASHnil(hs) || hb < lo) {
				o = i + seq;
				bunfastapp(bn, &o);
			}
		}
	} else {
		size_t nmelen;
		BUN prb;
		BUN p;
		BUN mask;

		ALGODEBUG fprintf(stderr, "#BATsubunique(b=%s#" BUNFMT ",s=%s#" BUNFMT "): create partial hash\n",
				  BATgetId(b), BATcount(b),
				  s ? BATgetId(s) : "NULL",
				  s ? BATcount(s) : 0);
		nme = BBP_physical(b->batCacheid);
		nmelen = strlen(nme);
		if (ATOMbasetype(b->ttype) == TYPE_bte) {
			mask = 1 << 8;
			cmp = NULL; /* no compare needed, "hash" is perfect */
		} else if (ATOMbasetype(b->ttype) == TYPE_sht) {
			mask = 1 << 16;
			cmp = NULL; /* no compare needed, "hash" is perfect */
		} else {
			if (s)
				mask = HASHmask(s->batCount);
			else
				mask = HASHmask(b->batCount);
			if (mask < (1 << 16))
				mask = 1 << 16;
		}
		if ((hp = GDKzalloc(sizeof(Heap))) == NULL ||
		    (hp->filename = GDKmalloc(nmelen + 30)) == NULL ||
		    snprintf(hp->filename, nmelen + 30,
			     "%s.hash" SZFMT, nme, MT_getpid()) < 0 ||
		    (ext = GDKstrdup(hp->filename + nmelen + 1)) == NULL ||
		    (hs = HASHnew(hp, b->ttype, BUNlast(b), mask, BUN_NONE)) == NULL) {
			if (hp) {
				if (hp->filename)
					GDKfree(hp->filename);
				GDKfree(hp);
			}
			if (ext)
				GDKfree(ext);
			hp = NULL;
			ext = NULL;
			GDKerror("BATsubunique: cannot allocate hash table\n");
			goto bunins_failed;
		}
		for (;;) {
			if (cand) {
				if (cand == candend)
					break;
				i = *cand++ - b->hseqbase;
				if (i >= end)
					break;
			} else {
				i = start++;
				if (i == end)
					break;
			}
			v = VALUE(i);
			prb = HASHprobe(hs, v);
			for (hb = HASHget(hs, prb);
			     hb != HASHnil(hs);
			     hb = HASHgetlink(hs, hb)) {
				if (cmp == NULL || cmp(v, BUNtail(bi, hb)) == 0)
					break;
			}
			if (hb == HASHnil(hs)) {
				o = i + b->hseqbase;
				p = i + BUNfirst(b);
				bunfastapp(bn, &o);
				/* enter into hash table */
				HASHputlink(hs, p, HASHget(hs, prb));
				HASHput(hs, prb, p);
			}
		}
		HEAPfree(hp, 1);
		GDKfree(hp);
		GDKfree(hs);
		GDKfree(ext);
	}

	bn->tsorted = 1;
	bn->trevsorted = BATcount(bn) <= 1;
	bn->tkey = 1;
	bn->T->nil = 0;
	bn->T->nonil = 1;
	return virtualize(bn);

  bunins_failed:
	if (seen)
		GDKfree(seen);
	if (hp) {
		HEAPfree(hp, 1);
		GDKfree(hp);
		GDKfree(hs);
		GDKfree(ext);
	}
	BBPreclaim(bn);
	return NULL;
}

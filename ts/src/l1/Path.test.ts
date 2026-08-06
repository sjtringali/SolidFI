// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Path } from './Path';

import { Converter } from './Converter';

type Point = { x: number; y: number } | null;
type Rect  = { x: number; y: number; width: number; height: number } | null;

class ParsePoint implements Converter<string, Point> {
    resolve(s: string): Point {
        const parts = s.split(',').map(Number);
        return parts.length === 2 && parts.every(Number.isFinite)
            ? { x: parts[0], y: parts[1] }
            : null;
    }
}

class ExpandToRect implements Converter<Point, Rect> {
    constructor(private readonly size: number) {}

    resolve(p: Point): Rect {
        return p && { x: p.x, y: p.y, width: this.size, height: this.size };
    }
}

describe('Path', () => {
    it('returns null by default when empty', () => {
        const path = new Path<string, Rect>();
        assert.equal(path.traverse('10,20', {}), null);
    });

    it('walks a multi-step path from string to Rect', () => {
        const step1 = new ParsePoint();      // string -> Point
        const step2 = new ExpandToRect(100); // Point -> Rect

        // Factory vararg variant to make a Path
        const path = Path.create<string, Rect>(step1, step2);
        assert.deepEqual(path.traverse('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
        assert.equal(path.traverse('bad', {}), null);
    });

    it('traverse() and resolve() return the same result', () => {
        const path = new Path<string, Rect>();
        path.append(new ParsePoint());
        path.append(new ExpandToRect(50));
        assert.deepEqual(path.traverse('5,10', {}), path.resolve('5,10', {}));
    });

    it('is assignable to Converter', () => {
        const path = Path.create<string, Rect>(new ParsePoint(), new ExpandToRect(10));
        const c: Converter<string, Rect> = path;
        assert.deepEqual(c.resolve('1,2', {}), { x: 1, y: 2, width: 10, height: 10 });
    });

    it('passes params through to each step', () => {
        type SizeParams = { size: number };

        class ExpandToRectParam implements Converter<Point, Rect, SizeParams> {
            resolve(p: Point, params: SizeParams): Rect {
                return p && { x: p.x, y: p.y, width: params.size, height: params.size };
            }
        }

        const path = Path.create<string, Rect, SizeParams>(new ParsePoint(), new ExpandToRectParam());
        assert.deepEqual(path.traverse('3,4', { size: 20 }), { x: 3, y: 4, width: 20, height: 20 });
        assert.equal(path.traverse('bad', { size: 20 }), null);
    });

    it('to() advances the type; failure at one step propagates through the rest', () => {
        const path = new Path<string, Point>()
            .to(new ParsePoint())
            .to(new ExpandToRect(100));
        assert.deepEqual(path.traverse('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
        assert.equal(path.traverse('bad', {}), null);
    });

    it('to() returns an independently usable subpath; extended path can substitute for the full type', () => {
        const subpath: Path<string, Point> = new Path<string, Point>().to(new ParsePoint());
        const full: Path<string, Rect> = subpath.to(new ExpandToRect(100));
        assert.deepEqual(subpath.traverse('10,20', {}), { x: 10, y: 20 });
        assert.deepEqual(full.traverse('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
        const converter: Converter<string, Rect> = full;
        assert.deepEqual(converter.resolve('10,20', {}), { x: 10, y: 20, width: 100, height: 100 });
    });

    it('propagates intermediate failure through remaining steps to the final result', { todo: true }, () => {
        // Path should guarantee failure propagation; each step must receive and forward the failure value.
        // Requires an interior builder that knows each step's failure value.
        const path = Path.create<string, Rect>(new ParsePoint(), new ExpandToRect(100));
        assert.equal(path.traverse('bad', {}), null); // ParsePoint returns null; ExpandToRect must propagate it
    });

    it('detects and throws on a step type mismatch (requires interior builder)', { todo: true }, () => {
        const path = new Path<string, Rect> ();
        path.append(new ExpandToRect(100)); // wrong: expects Point, gets string
        assert.throws(() => path.traverse('10,20', {}));
    });
});
